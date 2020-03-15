// Library required: https://github.com/nRF24/RF24Network/
// Library required: https://github.com/nRF24/RF24/

#include "SPI.h"
#include "RF24.h"
#include "RF24Network.h"

#define startPin 3

#define buttonPin 4
#define redLEDPin 5
#define greenLedPin 6

RF24 radio(7, 8);
RF24Network network(radio);

struct message_t {
  int type;
  unsigned long ms;
  uint16_t address;
};

const int channel = 90;
const uint16_t node_address = 1;

int state = 0;
//0: waiting to start
//1: playing, not pressed
//2: playing, pressed
//3: ended

unsigned long start_game;
int pressed_count = 0;
bool pressed = false;
bool organizer = false;

uint16_t other_players[6];
int player_count = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setting up");

  pinMode(startPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  SPI.begin();
  radio.begin();
  network.begin(channel, node_address);
}

void loop() {
  if (digitalRead(startPin) == LOW && !organizer) {
    organizer = true;
    send_data(1, 0, node_address);
  }

  if (digitalRead(buttonPin) == LOW && (state == 0 || !pressed)) {
    if (state == 0) {
      start_game = millis();
      state = 1;
    } else {
      pressed = true;
      send_data(2, millis() - start_game, node_address);
    }
  }
  
  network.update();
  while (network.available()) {
    RF24NetworkHeader header;
    message_t message;
    network.read(header, &message, sizeof(message));
    Serial.print("Received packet ");
    Serial.print(message.type);
    Serial.print(" ");
    Serial.print(message.ms);
    Serial.print(" ");
    Serial.println(message.address);

    switch (message.type) {
      case 0:
        other_players[player_count++] = message.address;
      break;
      case 1:
        start_game = millis();
      break;
      case 2:
        pressed_count++;
      break;
      case 3:
        digitalWrite(redLEDPin, LOW);
        if (pressed_number == 3) {
          digitalWrite(greenLEDPin, HIGH);
        }
        break;
    }
  }
}

void send_data(int type, unsigned long ms, uint16_t id) {
  //type 0: join game
  //type 1: start game
  //type 2: press button
  //type 3: end game
  message_t message = {type, ms, id};
  for (int i = 0;  i < 6; i++) {
    RF24NetworkHeader header(other_players[i]);
    Serial.print(other_players[i]);
    if (network.write(header, &message, sizeof(message))) {
      Serial.println("Write ok");
    } else {
      Serial.println("Write failed");
    }
  }
}
