#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET_PIN 4
#define BUTTON_PIN 5

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET_PIN);

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println("Failed to initialize display");
    while (true) {}
  }

  display.clearDisplay();
}

void loop() {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 20);
  display.cp437(true);
  display.print("Press the button to spin");
  display.display();
  
  if (pulseInLong(BUTTON_PIN, LOW, 1000 * 1000 * 60) == 0) {
    return;
  }

  const int smallRadius = 25;
  const int largeRadius = 50;
  
  display.drawCircle(display.width() / 2, display.height() / 2, smallRadius, SSD1306_WHITE);
  display.drawCircle(display.width() / 2, display.height() / 2, largeRadius, SSD1306_WHITE);
  for (int i = 0; i < 6; i++) {
    display.drawLine(display.width() / 2 + smallRadius * cos(i * PI / 6),
                     display.height() / 2 + smallRadius * sin(i * PI / 6),
                     display.width() / 2 + largeRadius * cos(i * PI / 6),
                     display.height() / 2 + largeRadius * sin(i * PI / 6),
                     SSD1306_WHITE
    );
  }
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  for (int i = 0; i < 6; i++) {
    display.setCursor(display.width() / 2 + (smallRadius + largeRadius) / 2);
    display.print(i + 1);
  }

  display.display();
}
