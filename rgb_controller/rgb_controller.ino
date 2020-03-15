#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "WiFiClient.h"
#include "ESP8266mDNS.h"

const char* SSID = "SSID-GOES-HERE";
const char* PASSWORD = "PASSWORD-GOES-HERE";

const String updateForm = "<html>\n\
\n\
<head>\n\
  <meta charset=\"UTF-8\">\n\
  <title>Web RGB Controller</title>\n\
</head>\n\
\n\
<body>\n\
  <h1>Set color and brightness of LED</h1>\n\
  <input type=\"color\" id=\"color\">\n\
</body>\n\
<script>\n\
  document.getElementById('color').addEventListener('change', () => {\n\
    const color = document.getElementById('color').value\n\
    fetch(`/color?red=${parseInt(color.substring(1,3), 16)}&green=${parseInt(color.substring(3,5), 16)}&blue=${parseInt(color.substring(5,7), 16)}`)\n\
  })\n\
\n\
</script>\n\
\n\
</html>";

const int redPin = 5;
const int greenPin = 4;
const int bluePin = 2;

ESP8266WebServer server(80);

void serverRoot() {
  server.send(200, "text/html", updateForm);
}

void serverColor() {
    analogWrite(redPin, server.arg("red").toInt() * 4);
    analogWrite(greenPin, server.arg("green").toInt() * 4);
    analogWrite(bluePin, server.arg("blue").toInt() * 4);
    server.send(200, "text/plain", "Ok");
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

  analogWriteFreq(10 * 1000);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("rgbled")) {
    Serial.println("mDNS enabled");
  }

  server.on("/", serverRoot);
  server.on("/color", serverColor);
  server.onNotFound(serverNotFound);

  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();
}
