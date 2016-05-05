// Encode input pins and starting position
int encoderA = 2;
int encoderB = 4;
int encoderPos = 0;
// Solenoid valve pin and opening time
int valvePin = 8;
int valveTime = 250;
// Auditory feedback pin
int speakerPin = 12;
// Position to reach for the mouse
int targetPos = 5000;
// Auditory feedback parameters
int toneFreq;
int tonePeriod = 50;
int tonePeriod_;

void setup() {
  pinMode (encoderA, INPUT);
  pinMode (encoderB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA), doEncoder, CHANGE);

  pinMode (valvePin, OUTPUT);
  pinMode (speakerPin, OUTPUT);

  Serial.begin (9600);
}

void doEncoder() {
  if (digitalRead(encoderA) == digitalRead(encoderB)) {
    encoderPos++;
  } else {
    if (encoderPos>0) {
      encoderPos--;
    }
  }
  toneFreq = (long)encoderPos * 100 / targetPos + 20;
  tonePeriod = 1000 / toneFreq;
  //Serial output takes too long. 
  //Serial.println (encoderPos, DEC);
}

void loop() {
  if (encoderPos >= targetPos) {
    digitalWrite (valvePin, HIGH);
    delay(valveTime);
    digitalWrite (valvePin, LOW);
    encoderPos = 0;
    // Disable interrupts in case one occurs while are writing...
    noInterrupts();
    tonePeriod = 50;
    interrupts();
  }
  // ...or reading
  noInterrupts();
  tonePeriod_ = tonePeriod;
  interrupts();
  // make a click
  digitalWrite(speakerPin, HIGH);
  delay(tonePeriod_);

  digitalWrite(speakerPin, LOW);
  delay(tonePeriod_);
}
