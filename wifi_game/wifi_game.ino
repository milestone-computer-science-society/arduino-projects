#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClient.h"

#define SSID "SSID-GOES-HERE"
#define PASSWORD "PASSWORD-GOES-HERE"

#define RED_LED_PIN 5
#define GREEN_LED_PIN 4
#define BUTTON_PIN 14

const char* connect_url = "http://comsci.ml:12002/connect";
const char* play_url = "http://comsci.ml:12002/play";

void(* softReset) (void) = 0;

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

void loop() {

  int timeout = pulseIn(BUTTON_PIN, LOW, 120 * 1000 * 1000);
  if (timeout == 0) {
    return;
  }
  WiFiClient client;
  HTTPClient http;
  http.setTimeout(120 * 1000);

  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);
  if (http.begin(client, connect_url)) {
    int code = http.GET();
    if (code == HTTP_CODE_OK) {
      http.end();
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      pulseIn(BUTTON_PIN, LOW, 120 * 1000 * 1000);
      if (http.begin(client, play_url)) {
        int code = http.GET();
        if (code == HTTP_CODE_OK) {
          String response = http.getString();
          Serial.println(response);
          http.end();
          if (response == "won") {
            digitalWrite(GREEN_LED_PIN, HIGH);
          } else {
            digitalWrite(RED_LED_PIN, HIGH);
          }
          delay(5000);
        } else {
          Serial.println(http.errorToString(code).c_str());
          softReset();
        }
      }
    } else {
      Serial.println(http.errorToString(code).c_str());
      softReset();
    }
  } else {
    Serial.println("connection failed, restarting");
    softReset();
  }
}
