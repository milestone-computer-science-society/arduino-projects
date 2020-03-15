//Library required https://github.com/beegee-tokyo/DHTesp

#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "DHTesp.h"

const char* SSID = "SSID-GOES-HERE";
const char* PASSWORD = "PASSWORD-GOES-HERE";

ESP8266WebServer server(80);
DHTesp dht;

void serverRoot() {
  String page = "<!DOCTYPE html>\n\
<html lang=\"en\" dir=\"ltr\">\n\
\n\
<head>\n\
  <meta charset=\"utf-8\">\n\
  <title>Smart thermometer</title>\n\
</head>\n\
\n\
<body>\n\
  <div>\n\
    Temperature: <span id=\"temperature\">Loading...</span>\n\
  </div>\n\
  <div>\n\
    Humidity: <span id=\"humidity\">Loading...</span>\n\
  </div>\n\
</body>\n\
<script>\n\
  let getTemperature = async () => {\n\
    const data = await fetch('/temperature')\n\
    const body = await data.json()\n\
    document.getElementById('temperature').textContent = `${body.temperature}°C`\n\
    setTimeout(getTemperature, 2000)\n\
  }\n\
  let getHumidity = async () => {\n\
    const data = await fetch('/humidity')\n\
    const body = await data.json()\n\
    document.getElementById('humidity').textContent = `${body.humidity}%`\n\
    setTimeout(getHumidity, 2000)\n\
  }\n\
  getTemperature()\n\
  getHumidity()\n\
\n\
</script>\n\
\n\
</html>";
  server.send(200, "text/html", page);
}

void serverTemperature() {
  float temperature = dht.getTemperature();
  char buffer[20];
  sprintf(buffer, "{\"temperature\":%f}", temperature);
  server.send(200, "text/plain", buffer);
}

void serverHumidity() {
  float humidity = dht.getHumidity();
  char buffer[20];
  sprintf(buffer, "{\"humidity\":%f}", humidity);
  server.send(200, "text/plain", buffer);
}

void serverNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);

  dht.setup(4, DHTesp::DHT22);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("thermometer")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", serverRoot);
  server.on("/temperature", serverTemperature);
  server.on("/humidity", serverHumidity);
  server.onNotFound(serverNotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}
