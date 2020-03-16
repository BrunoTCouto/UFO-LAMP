
// Circuits Sequence
// 14 0 15 5
int amarelo = 14;
int verde = 0;
int roxo = 15;
int azul = 5;

int laranja = 13;

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProDimSwitch.h"

#define WIFI_SSID         ""    
#define WIFI_PASS         ""
#define APP_KEY           ""
#define APP_SECRET        ""
#define DIMSWITCH_ID      ""
#define BAUD_RATE         9600

// we use a struct to store all states and values for our dimmable switch
struct {
  bool powerState = false;
  int powerLevel = 0;
} device_state;

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  device_state.powerState = state;
  return true; // request handled properly
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  device_state.powerLevel = powerLevel;
  Serial.printf("Device %s power level changed to %d\r\n", deviceId.c_str(), device_state.powerLevel);
  return true;
}

bool onAdjustPowerLevel(const String &deviceId, int levelDelta) {
  device_state.powerLevel += levelDelta;
  Serial.printf("Device %s power level changed about %i to %d\r\n", deviceId.c_str(), levelDelta, device_state.powerLevel);
  levelDelta = device_state.powerLevel;
  return true;
}

void setupWiFi() {
  pinMode(amarelo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(roxo, OUTPUT);
  pinMode(azul, OUTPUT);
  pinMode(laranja, OUTPUT);
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProDimSwitch &myDimSwitch = SinricPro[DIMSWITCH_ID];

  // set callback function to device
  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  if(device_state.powerState){
    if(device_state.powerLevel >= 65){
      digitalWrite(13, HIGH);
      digitalWrite(14, HIGH);
      delay(device_state.powerLevel);
      digitalWrite(14, LOW);
      digitalWrite(0, HIGH);
      delay(device_state.powerLevel);
      digitalWrite(0, LOW);
      digitalWrite(15, HIGH);
      delay(device_state.powerLevel);
      digitalWrite(15, LOW);
      digitalWrite(5, HIGH);
      delay(device_state.powerLevel);
      digitalWrite(5, LOW);
    } else {
      digitalWrite(amarelo, HIGH);
      digitalWrite(verde, HIGH);
      digitalWrite(roxo, HIGH);
      digitalWrite(azul, HIGH);
      digitalWrite(laranja, HIGH);
    }
  } else {
    digitalWrite(amarelo, LOW);
    digitalWrite(verde, LOW);
    digitalWrite(roxo, LOW);
    digitalWrite(azul, LOW);
    digitalWrite(laranja, LOW);
  }
}
