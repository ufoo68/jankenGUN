#include <M5Core2.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <AXP192.h>

AXP192 power;
  
const uint16_t kIrLed = 26;
  
IRsend irsend(kIrLed);
extern uint8_t jankengun[];

void feedback() {
  power.SetLDOEnable(3, true);
  delay(100);
  power.SetLDOEnable(3, false);
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  irsend.begin(); // IR初期化
  M5.Lcd.drawJpg(jankengun, 10409, 0, 0);
}
  
void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    irsend.sendNEC(0x08F71FE0UL);
    Serial.println("gu");
    feedback();
  }
  if (M5.BtnB.wasPressed()) {
    irsend.sendNEC(0x08F71EE1UL);
    Serial.println("choki");
    feedback();
  }
  if (M5.BtnC.wasPressed()) {
    irsend.sendNEC(0x08F71AE5UL);
    Serial.println("pa");
    feedback();
  }
}