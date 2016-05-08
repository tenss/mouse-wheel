// Encode input pins and starting position
#define encoderA 2
#define encoderB 4
int encoderPos = 0;

// Solenoid valve pin and opening time
#define valvePin 8
int valveTime = 250;

// Auditory feedback parameters
#define speakerPin 12 // auditory feedback pin
int toneFreq = 5;
int tonePeriod = 1000 / (toneFreq * 2);
int tonePeriod_;

// Speed measurement
int timer1_counter; // timer for polling elapsed time
int lastPos = 0;    // encoder position at last iteration of the speed loop 
int lastSpeed = 0;  // last measured speed
int goodSpeed = 0;  // number of recent iterations when speed fell within range
int speedMismatch;  // difference between current and target speed
#define goodSpeedTarget 10  // value for goodSpeed to reach for reward
#define minSpeed 200 // minimum target speed
#define maxSpeed 400 // maximum target speed

void setup() {
  // setup timer1 to overflow after 100 ms
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  // hard coded to overflow at 10 Hz
  timer1_counter = 59286;   // preload timer 65536-16MHz/256/10Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();  
  
  pinMode (encoderA, INPUT);
  pinMode (encoderB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderA), doEncoder, CHANGE);

  pinMode (valvePin, OUTPUT);
  pinMode (speakerPin, OUTPUT);

  Serial.begin (9600);
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  lastSpeed = encoderPos - lastPos;
  lastPos = encoderPos;
  if ((lastSpeed >= minSpeed) && (lastSpeed <= maxSpeed)) {
    goodSpeed++;
    speedMismatch = 0;
  } else {
    speedMismatch = max(lastSpeed - maxSpeed, minSpeed - lastSpeed);
    goodSpeed = 0;
  }

  Serial.println(speedMismatch, DEC);
  toneFreq = max((minSpeed - speedMismatch)*100 / minSpeed, 0) + 10;
  tonePeriod = 1000 / (toneFreq * 2);
}

void doEncoder() {
  if (digitalRead(encoderA) == digitalRead(encoderB)) {
    encoderPos++;
  } else {
    if (encoderPos>0) {
      encoderPos--;
    }
  }
}

void loop() {
  if (goodSpeed >= goodSpeedTarget) {
    // Disable interrupts in case one occurs while are writing...
    detachInterrupt(digitalPinToInterrupt(encoderA));
    
    digitalWrite (valvePin, HIGH);
    delay(valveTime);
    digitalWrite (valvePin, LOW);
    lastPos = 0;
    encoderPos = 0;
    tonePeriod = 50;
    goodSpeed = 0;
    
    attachInterrupt(digitalPinToInterrupt(encoderA), doEncoder, CHANGE);
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