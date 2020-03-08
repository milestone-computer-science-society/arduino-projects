#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

#define SSID "Dani"
#define PASSWORD "Daniisthebest"

AsyncWebServer server(80);

//AsyncWebServerRequest requests[10];
int players = 0;

long startGame = 0;

void setup() {
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(14, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.begin(SSID, PASSWORD);

  server.on("/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (startGame == 0) {
    //requests[players++] = request;
    Serial.println(request->url());
    } else {
      request->send(200, "text/plain", "already playing");
    }
  });

  server.on("/play", HTTP_GET, [](AsyncWebServerRequest *request) {
    
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (startGame == 0 && digitalRead(12) == LOW) {
    startGame = millis();
    for (int i = 0; i < players; i++) {
     // requests[i]->send(200, "text/plain", "starting");
    }
  }
}
