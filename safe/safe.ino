// Library needed: https://github.com/Chris--A/Keypad/releases/tag/3.1.1

#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

#define DEBUG true

const byte rowCount = 4;
const byte colCount = 4;

char keys[rowCount][colCount] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const int servoPin = 3;

const byte keypadRows[rowCount] = {10, 11, 12, 13};
const byte keypadCols[colCount] = {6, 7, 8, 9};

const int passwordMaxLength = 12;
const int defaultPasswordLength = 4;
const char defaultPassword[passwordMaxLength] = {'0', '0', '0', '0'};

int passwordLength;
char password[passwordMaxLength];

int keyCount = 0;
char keysPressed[passwordMaxLength];

const int servoOpen = 0;
const int servoClosed = 90;

const int EEPROMtest = 69;
const int EEPROMtestAddr = 10;
const int passwordLengthAddr = 11;
const int passwordAddr = 12;

bool passwordChangeMode = false;
bool newPasswordMode = false;

bool servoIsOpen = false;
const long long int servoAutoClose = 5 * 60 * 1000;
long long int servoOpenTime = 0;

Keypad keypad = Keypad(makeKeymap(keys), keypadRows, keypadCols, rowCount, colCount);
Servo servo;

void setup() {
  // put your setup code here, to run once:
  if (DEBUG) {
    Serial.begin(9600);
    while (!Serial) {}
  }
  
  servo.attach(servoPin);
  servo.write(servoClosed);
  if (DEBUG) {
    Serial.print("Attached servo to pin ");
    Serial.println(servoPin);
  }

  if (EEPROM.read(EEPROMtestAddr) != EEPROMtest) {
    if (DEBUG) {
      Serial.println("WARNING! EEPROM test failed, resetting to default password (0000)");
    }
    EEPROM.write(EEPROMtestAddr, EEPROMtest);
    EEPROM.write(passwordLengthAddr, defaultPasswordLength);
    for (int i = 0; i < defaultPasswordLength; i++) {
      EEPROM.write(passwordAddr + i, defaultPassword[i]);
    }
  } else {
    if (DEBUG) {
      Serial.println("EEPROM test completed, loading password data");
    }
  }
  passwordLength = EEPROM.read(passwordLengthAddr);
  for (int i = 0; i < passwordLength; i++) {
    password[i] = EEPROM.read(passwordAddr + i);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - servoOpenTime >= servoAutoClose) {
    if (DEBUG) {
      Serial.println("Auto closing door");
    }
    servoIsOpen = false;
    servo.write(servoClosed);
  }
  
  char key = keypad.getKey();

  if (key) {
    if (DEBUG) {
      Serial.print("Keypad read key: ");
      Serial.println(key);
    }
    
    if (key == '#') {
      if (DEBUG) {
        Serial.println("Retry password");
      }
      keyCount = 0;
      return;
    }

    if (key == '*' && servoIsOpen) {
      if (DEBUG) {
        Serial.println("Closing door");
      }
      servo.write(servoClosed);
      servoIsOpen = false;
      return;
    }

    if (key == '*') {
      if (newPasswordMode) {
        if (DEBUG) {
          Serial.println("Saving new password");
        }
        EEPROM.write(passwordLengthAddr, passwordLength);
        for (int i = 0; i < passwordLength; i++) {
          EEPROM.write(passwordAddr + i, password[i]);
        }
        keyCount = 0;
        return;
      }
      keysPressed[keyCount++] = key;
      return;
    }
    
    if (keys[0] == '*' && keys[1] == '*') {
      passwordChangeMode = true;
      keyCount = 0;
    }
    
    if (key == '0' || key == '1' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
      if (!newPasswordMode) {
        keysPressed[keyCount++] = key;
      } else {
        password[passwordLength++] = key;
      }
    }
    
    if (keyCount == passwordLength) {
      for (int i = 0; i < passwordLength; i++) {
        if (password[i] != keysPressed[i]) {
          if (DEBUG) {
            Serial.println("Invalid password received");
          }
          keyCount = 0;
          return;
        }
      }
      if (DEBUG) {
        Serial.println("Correct password received");
      }
      keyCount = 0;
      if (!passwordChangeMode) {
        if (DEBUG) {
          Serial.println("Opening door");
        }
        servoIsOpen = true;
        servoOpenTime = millis();
        servo.write(servoOpen);
      } else {
        if (DEBUG) {
          Serial.println("Now waiting for new password");
        }
        passwordLength = 0;
      }
    }
  }
}
