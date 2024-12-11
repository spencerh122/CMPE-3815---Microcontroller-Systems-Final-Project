#include <ServoTimer2.h>

// Program that drives the brushless motor with an ESC. Reads from a joystick with input to pin A0. ESC pin is 7.

const int potpin = A0;
const int ESCpin = 7;
// constants for mapping function
const int JOYSTICK_MIN = 0;
const int JOYSTICK_MAX = 1023;  // analogRead gives values 0-1023
const int SERVO_MIN = 0;
const int SERVO_MAX = 180;  // Max power setting to the ESC with 16V setup
ServoTimer2 ESC;
ServoTimer2 OTHERSERVO;

void setup() {
  pinMode(potpin, INPUT);
  pinMode(ESCpin, OUTPUT);
  pinMode(4, OUTPUT);
  OTHERSERVO.attach(4);
  ESC.attach(ESCpin);
  ESC.write(0);
  Serial.begin(9600);
}

void loop() {
  int throttle_setting = analogRead(potpin);
  int ESC_throttle = map(throttle_setting, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);
  Serial.print(throttle_setting);
  Serial.print('\t');
  Serial.println(ESC_throttle);
  ESC.write(ESC_throttle);

  delay(1000);
}
