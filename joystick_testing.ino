


const int joystick_x = A4;
const int joystick_y = A5;

// constants for mapping function
const int JOYSTICK_MIN = 0;
const int JOYSTICK_MAX = 1023;  // analogRead gives values 0-1023
const int SERVO_MIN = 0;
const int SERVO_MAX = 180;  // 180  degrees max servo deflection
const int MOTOR_MIN = 20;
const int MOTOR_MAX = 170;     // digitalRead gives values 0-255
const int THROTTLE_MIN = 512;  //neutral joystick y-axis = off position/zero throttle, less than neutral still returns zero throttle

// constants for dead stick position with joystick
const int LOWER_CENTER_BOUND = 502;
const int UPPER_CENTER_BOUND = 506;
const int CENTER_JOYSTICK_POSITION = 512;

int joystick_x_map = 0;
int joystick_y_map = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(joystick_x, INPUT);
  pinMode(joystick_y, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int x_joy = analogRead(joystick_x);
  int y_joy = analogRead(joystick_y);

  // if (y_joy <= 512) {
  //   y_joy = 512;  // contrains y-axis values for throttle from neutral to MAX position. Joystick y-axis values below the neutral position will still result in an analog value of 512.
  // }

  // if (UPPER_CENTER_BOUND >= x_joy && x_joy >= LOWER_CENTER_BOUND) { // neutral binds joystick x-axis to account for jitter/offset
  //   x_joy = CENTER_JOYSTICK_POSITION;
  // }

  joystick_x_map = map(x_joy, JOYSTICK_MIN, JOYSTICK_MAX, SERVO_MIN, SERVO_MAX);
  joystick_y_map = map(y_joy, THROTTLE_MIN, JOYSTICK_MAX, MOTOR_MIN, MOTOR_MAX);


  Serial.print("X-axis value: ");
  Serial.print(joystick_x_map);
  Serial.print(", REAL X-axis value: ");
  Serial.print(x_joy);
  Serial.print(", Y-axis value: ");
  Serial.print(joystick_y_map);
  Serial.print(", REAL Y-axis value: ");
  Serial.println(y_joy);
}
