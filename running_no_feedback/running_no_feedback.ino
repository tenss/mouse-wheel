int val;
int encoderA = 3;
int encoderB = 4;
int encoderPos = 0;
int encoderALast = LOW;
int valvePin = 8;
int valveTime = 250;
int n = LOW;
int targetPos = 10000;

void setup() {
  pinMode (encoderA, INPUT);
  pinMode (encoderB, INPUT);

  pinMode (valvePin, OUTPUT);

  Serial.begin (9600);
}

void loop() {
  n = digitalRead(encoderA);
  if ((encoderALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoderB) == LOW) {
      encoderPos--;
    } else {
      encoderPos++;
    }
    Serial.print (encoderPos);
    Serial.print ("/");

    if (encoderPos >= targetPos) {
      digitalWrite (valvePin, HIGH);
      delay(valveTime);
      digitalWrite (valvePin, LOW);
      encoderPos = 0;
    }
  }
  encoderALast = n;
}
