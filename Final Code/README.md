# Final Code

## This folder contains the functioning communication and control code used to fly the airplane.

### Files

- **Bluetooth_control.ino**
  - This program controls the airplane with a Bluetooth link and the Dabble library, using the Dabble app on a smartphone to transmit data to the onboard Arduino.

- **Onboardcontroller.ino**
  - This was intended to be the final version of the control communication method for the airplane, using a radio frequency link to communicate between the pilot on the ground and the plane in the air. These commands are mapped to the correct servo outputs and used to steer the airplane. Due to spotty radio communication caused by hardware limitations, it did not work consistently enough to be useful.

- **Transmittercode.ino**
  - This is the transmitter side of the RF link. Using two joysticks for input and a radio transmitter with a soldered antenna, we transmit code to the onboard receiver, which is processed and used by the Arduino running the Onboardcontroller.ino code.
