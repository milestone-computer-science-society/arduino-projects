const int columnCount = 2;
const int rowCount = 2;

const int columnPins[columnCount] = {2, 3, 4};
const int rowPins[rowCount] = {5, 6};

void setup() {
  for (int i = 0; i < columnCount; i++) {
    pinMode(columnPins[i], OUTPUT);
    digitalWrite(columnPins[i], HIGH);
  }
  for (int i = 0; i < rowCount; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }

  Serial.begin(9600);
}

char keys[columnCount][rowCount] = {
     {'1', '2', '3'},
     {'4', '5', '6'}
};

void loop() {
  for (int i = 0; i < columnCount; i++) {
    digitalWrite(columnPins[i], LOW);
    
    for (int j = 0; j < rowCount; j++) {
      if (digitalRead(rowPins[j]) == LOW) {
        Serial.println(keys[i][j]);
      }
    }
    
    digitalWrite(columnPins[i], HIGH);
  }

  delay(100);
}
