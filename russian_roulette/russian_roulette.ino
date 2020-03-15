const int ledCount = 6;
const int ledPins[ledCount] = {2, 3, 4, 5, 6, 7};
const int buzzerPin = 8;
const int buttonPin = 10;

const int tempo = 300;

void setup() {
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);

  randomSeed(analogRead(0) * analogRead(1));
}

void loop() {
  if (pulseIn(buttonPin, LOW, 10000) == 0) {
    return;
  }
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  Serial.println("Running");
  int stepCount = random(5, 8) * 6 + random(6);

  for (int s = 0; s <= stepCount; s++) {
    digitalWrite(ledPins[(s + ledCount - 1) % ledCount], LOW);
    digitalWrite(ledPins[s % ledCount], HIGH);
    delay(tempo / 10 + tempo / 50 * s);
  }
  if (stepCount % ledCount == 0) {
    Serial.println("- dead");
    digitalWrite(buzzerPin, HIGH);
    delay(3 * tempo);
    digitalWrite(buzzerPin, LOW);
  } else {
    Serial.println("- survived");
  }
}
