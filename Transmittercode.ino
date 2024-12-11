/*
* Shea McGregor, Spencer Hart
* Final Project
* RF transmitter/remote code
*/

// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages.
// with a simple ASK (amplitude shift keying) transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module.
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12.

// Import RadioHead ASK library.
#include <RH_ASK.h>

#include <SPI.h> // Not actually used but needed to compile.

// Initialize RadioHead library for object "rf_driver".
RH_ASK rf_driver;


// RH library expects RF transmitter module to be connected to digital pin 12.
// Constants for the joystick x and y-axis pins.
const int LEFT_joystick_x = A0; // Axis may not match pins because the joysticks are faulty.
const int LEFT_joystick_y = A1;
const int RIGHT_joystick_x = A4;
const int RIGHT_joystick_y = A5;

// Constants for the mapping function.
const int JOYSTICK_MIN = 0;
const int JOYSTICK_MAX = 1023; // AnalogRead gives values 0-1023.
const int SERVO_TX_MIN = 0; // 0 degress min servo deflection
const int SERVO_TX_MAX = 180; // 180 degrees max servo deflection
const int MOTOR_TX_MIN = 20; // Motor will start at digital value 20 to establish proper connection. (This means the motor should be ON upon startup!)
const int MOTOR_TX_MAX = 170; // digitalRead gives values 0-255. The motor seems to "max-out" at 170.
const int THROTTLE_MIN = 512; // Neutral left joystick y-axis = off position/zero throttle. A position less than neutral still returns zero throttle to ensure we don't have "negative" throttle values.

// Creating x and y-axis bounds for each joystick based on their resting/neutral position analog values.
const int LEFT_JOYSTICK_X_AXIS_LOWER_CENTER_BOUND = 502;
const int LEFT_JOYSTICK_X_AXIS_UPPER_CENTER_BOUND = 506;
const int RIGHT_JOYSTICK_X_AXIS_LOWER_CENTER_BOUND = 516;
const int RIGHT_JOYSTICK_X_AXIS_UPPER_CENTER_BOUND = 520;
const int RIGHT_JOYSTICK_Y_AXIS_LOWER_CENTER_BOUND = 518;
const int RIGHT_JOYSTICK_Y_AXIS_UPPER_CENTER_BOUND = 522;
const int CENTER_JOYSTICK_POSITION = 512; // Desired neutral analog value.

// Initializing variables used in the mapping function.
int joystick_x_map = 0;
int joystick_y_map = 0;

// Initializing variables used to hold joystick values.
uint8_t counter = 0;
uint8_t joystick_left_x_rudder = 0;
uint8_t joystick_left_y_throttle = 0;
uint8_t joystick_right_x_aileron = 0;
uint8_t joystick_right_y_elevator = 0;


void setup() {

// Initialize ASK object
rf_driver.init();

// Initialize joystick axis as inputs. 
pinMode(LEFT_joystick_x, INPUT);
pinMode(LEFT_joystick_y, INPUT);
pinMode(RIGHT_joystick_x, INPUT);
pinMode(RIGHT_joystick_y, INPUT);

}


void loop() {
// Reading joystick values, storing the values in new variables.
int LEFT_x_joy = analogRead(LEFT_joystick_x);
int LEFT_y_joy = analogRead(LEFT_joystick_y);
int RIGHT_x_joy = analogRead(RIGHT_joystick_x);
int RIGHT_y_joy = analogRead(RIGHT_joystick_y);

// Contrains y-axis values for THROTTLE from neutral to MAX position. Joystick y-axis values below the neutral position will still result in an analog value of 512.
if (LEFT_y_joy <= 512) {
LEFT_y_joy = 512;
}

// Neutral binds left joystick x-axis to account for jitter/offset for RUDDER.
if (LEFT_JOYSTICK_X_AXIS_UPPER_CENTER_BOUND >= LEFT_x_joy && LEFT_x_joy >= LEFT_JOYSTICK_X_AXIS_LOWER_CENTER_BOUND) {
LEFT_x_joy = CENTER_JOYSTICK_POSITION;
}


// Neutral binds right joystick x-axis to account for jitter/offset for AILERON.
if (RIGHT_JOYSTICK_X_AXIS_UPPER_CENTER_BOUND >= RIGHT_x_joy && RIGHT_x_joy >= RIGHT_JOYSTICK_X_AXIS_LOWER_CENTER_BOUND) {
RIGHT_x_joy = CENTER_JOYSTICK_POSITION;
}

// Neutral binds right joystick y-axis to account for jitter/offset for ELEVATOR.
if (RIGHT_JOYSTICK_Y_AXIS_UPPER_CENTER_BOUND >= RIGHT_y_joy && RIGHT_y_joy >= RIGHT_JOYSTICK_Y_AXIS_LOWER_CENTER_BOUND) {
RIGHT_y_joy = CENTER_JOYSTICK_POSITION;
}

// Mapping functions for LEFT joystick.
joystick_left_x_rudder = map(LEFT_x_joy, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_TX_MIN, SERVO_TX_MAX);
joystick_left_y_throttle = map(LEFT_y_joy, THROTTLE_MIN, JOYSTICK_MAX, MOTOR_TX_MIN, MOTOR_TX_MAX);

// Mapping functions for RIGHT joystick.
joystick_right_x_aileron = map(RIGHT_x_joy, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_TX_MIN, SERVO_TX_MAX);
joystick_right_y_elevator = map(RIGHT_y_joy, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_TX_MAX, SERVO_TX_MIN);

// Initializing "msg" as an array of 5 bytes of type char.
char msg[5];
// Giving values to each byte.
msg[0] = counter;
msg[1] = joystick_left_x_rudder;
msg[2] = joystick_left_y_throttle;
msg[3] = joystick_right_x_aileron;
msg[4] = joystick_right_y_elevator;

// Sending "msg" to receiver.
rf_driver.send((uint8_t *)msg, 5); // Last number indicates number of bytes to send (# of channels).
rf_driver.waitPacketSent(); // Wait until the whole message is gone.

// Incrementing counter.
counter++;
delay(100);
}

