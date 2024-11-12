#include <SoftwareSerial.h>
#include <Servo.h>

const int RX_PIN = 3;
const int TX_PIN = 4;
SoftwareSerial BTSerial(RX_PIN, TX_PIN);

Servo miServo;
const int SERVO_PIN = 27;

const int LIGHT_SENSOR_PIN = A0;
const int LIGHT_THRESHOLD = 500;
const int LIGHT_LEDS[] = {28, 29, 30, 31, 32, 33};
const int NUM_LIGHT_LEDS = 6;

const int NUM_LEDS = 25;
const int ledPins[NUM_LEDS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
const int DELAY_TIME = 100;

void setup() {
  Serial.begin(9600);
  setupBluetooth();
  setupLEDs();
  setupServo();
  setupLightSensor();
}

void setupServo() {
  miServo.attach(SERVO_PIN);
  miServo.write(0);
}

void setupLightSensor() {
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  for (int i = 0; i < NUM_LIGHT_LEDS; i++) {
    pinMode(LIGHT_LEDS[i], OUTPUT);
    digitalWrite(LIGHT_LEDS[i], LOW);
  }
}

void setupBluetooth() {
  BTSerial.begin(9600);
}

void setupLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void handleLightSensor() {
  int lightValue = analogRead(LIGHT_SENSOR_PIN);

  if (lightValue < LIGHT_THRESHOLD) {
    for (int i = 0; i < NUM_LIGHT_LEDS; i++) {
      digitalWrite(LIGHT_LEDS[i], LOW);
    }
  } else {
    for (int i = 0; i < NUM_LIGHT_LEDS; i++) {
      digitalWrite(LIGHT_LEDS[i], HIGH);
    }
  }
}

void moveServoForward(int targetAngle) {
  int currentAngle = miServo.read();
  for (int angle = currentAngle; angle <= targetAngle; angle++) {
    miServo.write(angle);
    delay(15);
  }
}

void moveServoBackward(int targetAngle) {
  int currentAngle = miServo.read();
  for (int angle = currentAngle; angle >= targetAngle; angle--) {
    miServo.write(angle);
    delay(15);
  }
}

void handleBluetooth() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    BTSerial.println(data);
    processCommand(data);
  }

  if (BTSerial.available()) {
    String data = BTSerial.readStringUntil('\n');
    Serial.println(data);
    processCommand(data);
  }
}

void processCommand(String command) {
  if (command.startsWith("S")) {
    int angle = command.substring(1).toInt();
    if (angle >= 0 && angle <= 180) {
      if (angle > miServo.read()) {
        moveServoForward(angle);
      } else {
        moveServoBackward(angle);
      }
    }
  } else {
    switch (command[0]) {
      case '1':
        secuenciaAscendente();
        break;
      case '2':
        secuenciaCentroLados();
        break;
      case '3':
        secuenciaDescendente();
        break;
      case '0':
        apagarTodos();
        break;
    }
  }
}

void secuenciaAscendente() {
  apagarTodos();
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(DELAY_TIME);
  }
}

void secuenciaCentroLados() {
  apagarTodos();
  int centro = NUM_LEDS / 2;
  int izquierda = centro - 1;
  int derecha = centro + 1;

  digitalWrite(ledPins[centro], HIGH);
  delay(DELAY_TIME);

  while (izquierda >= 0 || derecha < NUM_LEDS) {
    if (izquierda >= 0) {
      digitalWrite(ledPins[izquierda], HIGH);
    }
    if (derecha < NUM_LEDS) {
      digitalWrite(ledPins[derecha], HIGH);
    }
    delay(DELAY_TIME);
    izquierda--;
    derecha++;
  }
}

void secuenciaDescendente() {
  apagarTodos();
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    digitalWrite(ledPins[i], HIGH);
    delay(DELAY_TIME);
  }
}

void apagarTodos() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  handleBluetooth();
  handleLightSensor();
}
