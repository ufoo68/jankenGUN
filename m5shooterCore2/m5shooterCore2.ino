#include <M5Core2.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <AXP192.h>

AXP192 power;
  
const uint16_t kIrLed = 26;
  
IRsend irsend(kIrLed);
extern uint8_t jankengun[];
extern uint8_t select_gu_raw[];
extern unsigned int select_gu_raw_len;
extern uint8_t select_choki_raw[];
extern unsigned int select_choki_raw_len;
extern uint8_t select_pa_raw[];
extern unsigned int select_pa_raw_len;

void setup() {
  M5.begin();
  Serial.begin(115200);
  irsend.begin(); // IR初期化
  M5.Lcd.drawJpg(jankengun, 10409, 0, 0);
  M5.Spk.begin();
}
  
void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    irsend.sendNEC(0x08F71FE0UL);
    Serial.println("gu");
    M5.Spk.PlaySound(select_gu_raw, select_gu_raw_len);
  }
  if (M5.BtnB.wasPressed()) {
    irsend.sendNEC(0x08F71EE1UL);
    Serial.println("choki");
    M5.Spk.PlaySound(select_choki_raw, select_choki_raw_len);
  }
  if (M5.BtnC.wasPressed()) {
    irsend.sendNEC(0x08F71AE5UL);
    Serial.println("pa");
    M5.Spk.PlaySound(select_pa_raw, select_pa_raw_len);
  }
}