# Developmental Code

## This folder contains the developmental code we used while exploring new hardware and communications methods with our project.

### Files

- **MPU6050_attempt_to_read.ino**
  - This program was a developmental step during which time we attempted to obtain useful orientation data from the onboard IMU. We sought to do this by combining the MPU6050_raw example code by ElectronicCats (https://github.com/ElectronicCats/mpu6050/tree/master/examples/MPU6050_raw) with the Madgwick function of dRehmFlight (https://github.com/nickrehm/dRehmFlight/tree/master/Versions/dRehmFlight_Teensy_BETA_1.3). We suspect hardware malfunctions made this data unusable, as the IMU proved unreliable and would frequently freeze after just a few seconds of transmitting.

- **Motor_driver.ino**
  - This program was used to initially establish communication between the Arduino and the ESC by feeding the ESC a servo-style PWM signal with the Servo.h library. After establishing communication with the ESC, we used this program to zero our flight control servos before installation on the airplane.

- **joystick_receiver_MAPPED_VALS.ino**
  - This program was used to test the RF receiver module. It received channel data from the RF transmitter, checked for packet errors, then printed the corresponding channel values to the serial monitor.

- **joystick_sender_MAPPED_VALS.ino**
  - This program was used to test the RF transmitter module. It took in joystick values, made sure a neutral joystick position outputted the desired neutral position value, mapped the values to servo positions, then sent that data to the RF receiver module.

- **joystick_testing.ino**
  - This program was used to test that the joysticks functioned properly and ensure that mapping to servo positions could be done.
