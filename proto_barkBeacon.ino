// Nathan Vititoe
// Bark Beacon Test Template
/*
This template is built for use with a push btn in place of a microphone to simulate bark events
The LED should be lit up continuously whenever the motor is on, and off whenever the motor is off 
*/
#include <Bounce2.h>

const int pushBtn = 5;  // D5
Bounce debouncer = Bounce(); // btn debouncer

const int led = 2;      // D2
const int motor = 9;    // D9

void setup() {
  // init serial monitor
  Serial.begin(9600);

  // init components
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);

  // init pushbtn/debounce
  pinMode(pushBtn, INPUT);
  debouncer.attach(pushBtn); // attach debouncer to pushBtn
  debouncer.interval(25);   // debounce every 25ms

  // test components
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(motor, HIGH);
  delay(2000);

  // start motor/led as off
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(motor, LOW);
  digitalWrite(led, LOW);
  Serial.println("Setup Complete.");
}

void loop() {
  // update debouncer
  debouncer.update();

  // detect btn press
  if(debouncer.fell()) {
    Serial.println("bark");
    barkEvent();
  }
  int btnState = digitalRead(pushBtn);
  Serial.println(btnState);
}

// what to run when a bark occurs
void barkEvent() {
  Serial.println("Bark Event Occurred.");
  // 2s bark correction
  digitalWrite(led, HIGH);    // turn on bark/motor light
  digitalWrite(motor, HIGH);  // vibrate 2s

  // reset light and motor (turn off)
  digitalWrite(motor, LOW);
  digitalWrite(led, LOW);
}
