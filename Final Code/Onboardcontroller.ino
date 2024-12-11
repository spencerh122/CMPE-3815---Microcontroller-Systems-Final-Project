/*
* Shea McGregor, Spencer Hart
* Final Project
* RF receiver code
*/

// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK (amplitude shift keying) transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module.
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12.

// Servo library
#include <ServoTimer2.h>


// Import RadioHead ASK library.
#include <SPI.h>  // Not actually used but needed to compile.
#include <RH_ASK.h>

// Initialize RadioHead library for object "rf_driver".
RH_ASK rf_driver;


// RH library expects RF receiver module to be connected to digital pin 11.
// Initializing variables used to hold joystick values.
uint16_t counter = 0;
uint16_t joystick_left_x_rudder = 0;
uint16_t joystick_left_y_throttle = 0;
uint16_t joystick_right_x_aileron = 0;
uint16_t joystick_right_y_elevator = 0;

// Variables to command servos
int rudder_command;
int throttle_command;
int aileron_command;
int elevator_command;


// Servo control constants
// ServoTimer2 uses a different scale than the usual Servo library.
// Values for the receiver to output:
const int SERVOMID = 1500;
const int SERVO_RX_MIN = 750;
const int SERVO_RX_MAX = 2250;
// Values that the transmitter sends:
const int SERVO_TX_MIN = 0;
const int SERVO_TX_MAX = 180;

// Initializing variables used to hold inforamton about packets.
int currentPacket = 0;
uint8_t errorRate = 0;
uint8_t expectedPacket = 0;

// Initializing variables used to keep track of time.
long lastReceive = millis();                  // Used with respect to incoming packets.
long valueUpdate = millis();                  // Used with respect to sending joystick values to desired location (simulation: to the serial monitor, reality: to the servos).
long throttle_error_adjust_timer = millis();  // Used with respect to landing the plane upon high packet error.

// Pin declarations
const int ESCpin = 9;
const int rudder_pin = 6;
const int elevator_pin = 4;
const int left_aileron_pin = 7;
const int right_aileron_pin = 8;

// Control servos and main motor
ServoTimer2 ESC;
ServoTimer2 left_aileron;
ServoTimer2 right_aileron;
ServoTimer2 elevator;
ServoTimer2 rudder;

void setup() {
  // Initialize ASK object.
  rf_driver.init();
  // Setup serial monitor.
  Serial.begin(9600);

  // Attach servos to their corresponding pins
  ESC.attach(ESCpin);
  rudder.attach(rudder_pin);
  elevator.attach(elevator_pin);
  left_aileron.attach(left_aileron_pin);
  right_aileron.attach(right_aileron_pin);
}


void loop() {

  // Initializing "buf" (buffer) as an array to hold 5 bytes/channels.
  uint8_t buf[5];
  uint8_t buflen = sizeof(buf);  // Variable to hold length/size of array.

  // Initializing boolean variable to keep track of errors. Starts at a value of NO error.
  bool isError = false;

  // The case of receiving a message. Giving names to each byte in the message.
  if (rf_driver.recv(buf, &buflen)) {
    currentPacket = (uint8_t)buf[0];
    joystick_left_x_rudder = (uint8_t)buf[1];
    joystick_left_y_throttle = (uint8_t)buf[2];
    joystick_right_x_aileron = (uint8_t)buf[3];
    joystick_right_y_elevator = (uint8_t)buf[4];

    // Starting timer for "received message" case.
    lastReceive = millis();

    // Map received servo values to the servo values that we can actually use with this library
    rudder_command = map(joystick_left_x_rudder, SERVO_TX_MIN, SERVO_TX_MAX, SERVO_RX_MIN, SERVO_RX_MAX);
    throttle_command = map(joystick_left_y_throttle, SERVO_TX_MIN, SERVO_TX_MAX, SERVO_RX_MIN, SERVO_RX_MAX);
    aileron_command = map(joystick_right_x_aileron, SERVO_TX_MIN, SERVO_TX_MAX, SERVO_RX_MIN, SERVO_RX_MAX);
    elevator_command = map(joystick_right_y_elevator, SERVO_TX_MIN, SERVO_TX_MAX, SERVO_RX_MIN, SERVO_RX_MAX);



    // Add to error if the packet number does not match the expected packet value. Initially, it expects a packet value of 0 (denoting the first packet sent).
    if (currentPacket != expectedPacket) {
      isError = true;
      errorRate++;
    }

    // Will now expect to receive the next packet value.
    expectedPacket = currentPacket + 1;

    //If there was no error, decrease error counter.
    if (!isError) {
      if (errorRate > 0) {
        errorRate--;
      }
    }
  }
  // No message was received after a period of time, increase error counter.
  if (!isError && (millis() - lastReceive > 200)) {
    errorRate++;
    isError = true;
    lastReceive = millis();
  }

  // Straighten plane and slowly decrease throttle to 0 if the error becomes too great (meaning we begin to lose connection).
  if (errorRate > 10 && (millis() - throttle_error_adjust_timer > 350)) {
    joystick_left_x_rudder = SERVOMID;
    joystick_right_x_aileron = SERVOMID;
    joystick_right_y_elevator = SERVOMID;
    if (joystick_left_y_throttle > 10) {
      joystick_left_y_throttle = joystick_left_y_throttle - 1;
    } else {
      joystick_left_y_throttle = 0;
    }
    throttle_error_adjust_timer = millis();
  }



  // Error counter should not go past 100.
  if (errorRate > 100) {
    errorRate = 100;
  }

  //Sending joystick values to desired location.
  if (millis() - valueUpdate > 150) {

    Serial.print("Packet: ");
    Serial.print(currentPacket);

    Serial.print(", Error rate: ");
    Serial.print(errorRate);

    Serial.print(" | Rudder position: ");
    Serial.print(joystick_left_x_rudder);

    Serial.print(" | Throttle: ");
    Serial.print(joystick_left_y_throttle);

    Serial.print(" | Aileron position: ");
    Serial.print(joystick_right_x_aileron);

    Serial.print(" | Elevator position: ");
    Serial.println(joystick_right_y_elevator);
    Serial.println();

    valueUpdate = millis();
  }

  move_controls();
}

void move_controls() {
  rudder.write(rudder_command);
  ESC.write(throttle_command);
  left_aileron.write(aileron_command);
  right_aileron.write(aileron_command);  // Aileron servos face opposite directions so they can get the same command
  elevator.write(elevator_command);
}
