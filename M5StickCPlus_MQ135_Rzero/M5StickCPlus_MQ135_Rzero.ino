#include <M5StickCPlus.h>
#include "MQ135.h"

// MQ135 Pin config
#define AO_IN        36
#define DO_IN        26

MQ135 gasSensor = MQ135(AO_IN); // Attach sensor to pin AO_IN

void setup (){
  // void begin(bool LCDEnable=true, bool PowerEnable=true, bool SerialEnable=true);
  M5.begin();
  // Wire.begin(32,33);
  M5.Lcd.setRotation(3);

  // SET INPUT PINS
  pinMode(AO_IN, INPUT);
  pinMode(DO_IN, INPUT);
}
void loop() {
  float rzero = gasSensor.getRZero();
  Serial.println (rzero);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("RZERO: ");
  M5.Lcd.println(rzero);

  delay(1000);
}
