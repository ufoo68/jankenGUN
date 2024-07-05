#include <M5Core2.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <String>

#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiMulti wifiMulti;
HTTPClient http;

// GROVE PORT.Bを使う場合は36
const uint16_t kRecvPin = 36;

const uint32_t IRcode[] = {0x08F71FE0, 0x08F71EE1, 0x08F71AE5}; // {グー、チョキ、パー}

extern uint8_t gu[];
extern uint8_t choki[];
extern uint8_t pa[];
extern uint8_t win[];
extern uint8_t lose[];
extern uint8_t draw[];
extern uint8_t jankengun[];
// サウンド用
// extern unsigned char draw_sound_raw[];
// extern unsigned int draw_sound_raw_len;
// extern unsigned char win_sound_raw[];
// extern unsigned int win_sound_raw_len;
// extern unsigned char lose_sound_raw[];
// extern unsigned int lose_sound_raw_len;

extern char *ssid;
extern char *password;

IRrecv irrecv(kRecvPin);
decode_results results;

String getState()
{
  DynamicJsonDocument getDoc(128);
  http.begin("http://163.43.185.149:1880/state");
  http.GET();
  String payload = http.getString();
  deserializeJson(getDoc, payload);
  String state = getDoc["state"];
  http.end();
  return state;
}

void post(String result)
{
  String state = getState();
  Serial.print("current state: ");
  Serial.println(state);
  if (state == "start") {
    DynamicJsonDocument postDoc(128);
    postDoc["result"] = result;
    char postData[128];
    serializeJson(postDoc, postData);
    http.begin("http://163.43.185.149:1880/hit");
    http.addHeader("Content-Type", "application/json");
    http.POST((uint8_t*)postData, strlen(postData));
    Serial.println("send result");
    http.end();
  }
}

void setup()
{
  M5.begin();
  M5.Lcd.drawJpg(jankengun, 9050, 0, 0);
  // M5.Spk.begin();
  Serial.begin(115200);
  irrecv.enableIRIn();
  while (!Serial) delay(50);
  wifiMulti.addAP(ssid, password);
  Serial.println("wifi connecting");
  while (wifiMulti.run() != WL_CONNECTED) delay(50);
  Serial.println("wifi connected");
  randomSeed(analogRead(0));
}

int janken;
unsigned long old_time = millis();
void loop()
{
  unsigned long current_time = millis();
  if (current_time - old_time >= 2000)
  {
    // 0から2の乱数を生成
    janken = random(0, 100) % 3;
    switch (janken)
    {
    // グー
    case 0:
      M5.Lcd.clear(BLACK);
      M5.Lcd.drawJpg(gu, 4546, 0, 0);
      Serial.println("gu");
      break;
    // チョキ
    case 1:
      M5.Lcd.clear(BLACK);
      M5.Lcd.drawJpg(choki, 4546, 0, 0);
      Serial.println("choki");
      break;
    // パー
    case 2:
      M5.Lcd.clear(BLACK);
      M5.Lcd.drawJpg(pa, 4546, 0, 0);
      Serial.println("pa");
      break;
    }
    old_time = millis();
  }
  if (irrecv.decode(&results))
  { // 受信したら
    // 　引き分け
    if (results.value == IRcode[janken])
    {
      Serial.println("draw");
      post("draw");
      M5.Lcd.drawJpg(draw, 7125, 0, 0);
      // M5.Spk.PlaySound(draw_sound_raw, draw_sound_raw_len);
    }
    // 勝ち
    else if (results.value == IRcode[0] && janken == 1 ||
             results.value == IRcode[1] && janken == 2 ||
             results.value == IRcode[2] && janken == 0)
    {
      Serial.println("win");
      post("win");
      M5.Lcd.drawJpg(win, 7416, 0, 0);
      // M5.Spk.PlaySound(win_sound_raw, win_sound_raw_len);
    }
    // 負け
    else if (results.value == IRcode[0] ||
             results.value == IRcode[1] ||
             results.value == IRcode[2])
    {
      Serial.println("lose");
      post("lose");
      M5.Lcd.drawJpg(lose, 7692, 0, 0);
      // M5.Spk.PlaySound(lose_sound_raw, lose_sound_raw_len);
    }
    // うまく読めていないor別の信号を受け取った
    else
    {
      Serial.println("recieved incorrect code");
    }
    irrecv.resume(); // Receive the next value
  }
}
