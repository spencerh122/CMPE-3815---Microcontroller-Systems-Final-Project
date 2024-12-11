#include <Servo.h>
// Bluetooth information
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE   // for control of the servos
#define INCLUDE_TERMINAL_MODULE  // for reading Bluetooth serial information
#include <Dabble.h>              // Dabble library
char buffer[32];                 // buffer for data transfer

// Program that drives the brushless motor with an ESC. Reads from a joystick with input to pin A0. ESC pin is 7.

const int ESCpin = 9;
const int rudder_pin = 5;
const int elevator_pin = 4;
const int left_aileron_pin = 7;
const int right_aileron_pin = 8;

// constants for mapping functions
const int JOYSTICK_MIN = -7;
const int JOYSTICK_MAX = 7;  // bluetooth joystick returns -7 to 7
const int SERVO_MIN = 0;
const int SERVO_MAX = 180;
const int THROTTLE_MIN = 18;   // Does not move motor, but sends signal to ESC to keep connected to the Arduino
const int THROTTLE_MAX = 170;  // Max power setting to the ESC with 16V setup
Servo ESC;
Servo left_aileron;
Servo right_aileron;
Servo elevator;
Servo rudder;
int throttle_setting;
int roll_command;
int pitch_command;
int yaw_command;

// Debounce BT commands - not all implemented yet
int last_rudder = 90;
int last_aileron = 0;
int last_elevator = 0;
int last_throttle = 0;
const float debounce_gap = .4;

void setup() {
  ESC.attach(ESCpin);
  left_aileron.attach(left_aileron_pin);
  right_aileron.attach(right_aileron_pin); 
  rudder.attach(rudder_pin);
  elevator.attach(elevator_pin);
  ESC.write(0);
  Serial.begin(9600);

  // setting up Bluetooth
  Dabble.begin(9600);  // set BT rate
}

void loop() {


  Dabble.processInput();  // Read from bluetooth



  // Data from gamepad
  /**********************************************************************
  Notes: angle is 0 when joystick is pointed right.
  angle is 90 when joystick is pointed up.
  angle is 180 when joystick is pointed left.
  angle is 270 when joystick is pointed down.
  x-axis data and y-axis data are centered (0,0) at the middle position.
  +x axis is to the right and +y axis is up.
  x and y value ranges: [-7,7].
  radius value range is [0,7]
  ***********************************************************************/


  // input variables
  float roll_input = GamePad.getXaxisData();
  float pitch_input = GamePad.getYaxisData();
  int yaw_input;

  // get input commands
  if (GamePad.isTrianglePressed()) {
    throttle_setting += 1;
    Serial.println("triangle");
  }
  if (GamePad.isCrossPressed()) {
    throttle_setting -= 5;
  }
  if (GamePad.isSquarePressed()) {
    yaw_input -= 10;
  }
  if (GamePad.isCirclePressed()) {
    yaw_input += 10;
  }
  if (GamePad.isStartPressed()) {
    throttle_setting = 18;
  }
  if (GamePad.isSelectPressed()) {
    yaw_input = 90;
  }

  // adjust input commands to match what we can actually output
  throttle_setting = constrain(throttle_setting, THROTTLE_MIN, THROTTLE_MAX);
  yaw_command = constrain(yaw_input, SERVO_MIN, SERVO_MAX);

  // (debounce pitch and roll commands)
  if (abs(pitch_input - last_elevator) > debounce_gap) {
    pitch_command = map(pitch_input, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);
    last_elevator = pitch_input;
  }
  if (abs(roll_input - last_aileron) > debounce_gap) {
    roll_command = map(roll_input, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MAX, SERVO_MIN);
    last_aileron = roll_input;
  }

  // output to the flight controls
  ESC.write(throttle_setting);
  left_aileron.write(roll_command);
  right_aileron.write(roll_command);  // Ailerons face opposite directions so they can get the same command
  elevator.write(pitch_command);
  rudder.write(yaw_command);

  Serial.println(roll_command);



  // int ESC_throttle = map(throttle_setting, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);

  // Serial.print(throttle_setting);
  // Serial.print('\t');
  // Serial.println(ESC_throttle);
  // ESC.write(ESC_throttle);
}
