/*
  Arduino MQ135
  connect the sensor as follows :
  A H A   >>> 5V
  B       >>> A0
  H       >>> GND
  B       >>> 22K ohm >>> GND
*/
/***************************************************
 * M5StackCPlus_MQ135_CO2.ino
 * 2021/02/11
 * LCD: 135 * 240 
 * Customized for M5StickCPlus by @tomorrow56
****************************************************/
#include <M5StickCPlus.h>
#include "MQ135.h"

// WiFi instanse
#include <WiFi.h>
#include <DNSServer.h>     // https://github.com/zhouhan0126/DNSServer---esp32
#include <WebServer.h>     // https://github.com/zhouhan0126/WebServer-esp32
#include "WiFiManager.h"   // https://github.com/zhouhan0126/WIFIMANAGER-ESP32

WiFiManager wifiManager;
WiFiClient client;

IPAddress ipadr;

// ambient key and event triggers are  
#include "Ambient.h"
Ambient ambient;

unsigned int channelId = *****; // AmbientのチャネルID
const char* writeKey = "****************"; // ライトキー

uint32_t update_time = 0;
uint32_t interval = 1000;
uint32_t update_time_ambient = 0;
uint32_t interval_ambient = 1000 * 60 * 5;

// MQ135 Pin config
#define AO_IN        36
float ppm = 0;

MQ135 gasSensor = MQ135(AO_IN);

void setup(void) {
  // void begin(bool LCDEnable=true, bool PowerEnable=true, bool SerialEnable=true);
  M5.begin();
  // Wire.begin(32,33);
  M5.Lcd.setRotation(3);

  // SET INPUT PINS
  pinMode(AO_IN, INPUT);

  // WiFiManager debug message enabled
  wifiManager.setDebugOutput(true);

  // WiFiManager auto connect setting check
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.println("AP:M5SickC-AP");
  M5.Lcd.println("IP:192.168.4.1");
  wifiManager.autoConnect("M5StickC-AP");

  //if you get here you have connected to the WiFi
  ipadr = WiFi.localIP();
  Serial.println("connected!");
  Serial.println(WiFi.SSID());
  Serial.println(ipadr);

  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.println("Wifi Connected!");
  M5.Lcd.println(" SSID:" + WiFi.SSID());
  M5.Lcd.println(" IP:" + (String)ipadr[0] + "." + (String)ipadr[1] + "." + (String)ipadr[2] + "." + (String)ipadr[3]);

  delay(500);

  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);

  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
  M5.Lcd.println("ambient begin...");
  M5.Lcd.print(" channelID:");
  M5.Lcd.println(channelId);
  M5.Lcd.print(" writeKey:");
  Serial.println(writeKey);

  delay(500);

  M5.Lcd.fillScreen(TFT_BLACK);
  printTitle("MQ135 CO2 Monitor");
  printItems();

  //1回目のデータは空読みして捨てる
  ppm = gasSensor.getPPM();
  delay(1000);
}

void loop() {
  M5.update();

  if(M5.BtnA.wasPressed()) {
    esp_restart();  
  }

  if(millis() > update_time){
    update_time = millis() + interval;

    ppm = gasSensor.getPPM();
    Serial.print ("ppm: ");
    Serial.println (ppm);

    if(ppm <= 1000){
      M5.Lcd.setTextColor(GREEN);
    }else if(ppm <= 1500){
      M5.Lcd.setTextColor(YELLOW);
    }else{
      M5.Lcd.setTextColor(RED);
    }
    printData(ppm);
  }

  if(millis() > update_time_ambient){
    update_time_ambient = millis() + interval_ambient;
    // 二酸化炭素濃度をAmbientに送信する
    ambient.set(1, String(ppm).c_str());
    ambient.send();
 
    Serial.printf("ambient: %.2f\r\n", ppm);
  }
}

void printTitle(String title){
  M5.Lcd.fillRect(0,0,240,20,BLUE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE,BLUE);
  M5.Lcd.drawCentreString(title,120,0,1);
}

void printItems(void){
  M5.Lcd.setTextColor(ORANGE);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10,30);
  M5.Lcd.print("CO2 concentration");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(200, 100);
  M5.Lcd.print("ppm");
}

void printData(float meas){
  M5.Lcd.setTextSize(4);
  M5.Lcd.fillRect(15,65,180,40,BLACK);
  M5.Lcd.drawRightString((String)meas,195,70,1);
}
