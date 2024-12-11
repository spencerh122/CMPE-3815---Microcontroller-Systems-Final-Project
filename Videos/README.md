# Videos

## This folder contains videos of our three projects under development. Read below for analyses of what did not work on each and what we learned.

- **Version 1 Radio Control**
  - This is how we controlled the first airplane in flight. We used a commercial transmitter and receiver to directly control the servo positions, as well as a commercial, lightweight, 22.2V LiPo battery.

- **Version 1 Flight**
  - This was a proof of concept flight using a conventional, commercial transmitter and receiver. This flight proved that our planned method of controlling the plane in flight with four servos and an ESC for the motor would work if we could get clear communication between the transmitter and the receiver. This design had the airplane's center of gravity too far aft, making it difficult to control and causing a crash after a few seconds of flight.

- **Version 2 Bluetooth Control**
  - After building a new airplane following Version 1's crash, we successfully implemented Arduino-only control using Bluetooth without the need for a commercial transmitter / receiver pair. However, due to the added weight of extra electronics, a customized 14.8V battery pack that was too heavy, and the high drag of the stepped airfoil design, this plane was not able to fly. We were able to implement the exact same Bluetooth control into Vehicle 3 soon after. However, it was too jittery and laggy in both cases to be practical in a flying aircraft.

- **Version 3 Low Power Flight**
  - Aircraft 3 was designed to be lightweight, but even so, the battery pack weighed too much for the amount of voltage that it was able to output. This flight demonstrates brief, controlled flight; however, due to the heavy battery and low available voltage, the airplane simply did not have sufficient thrust to fly more than a few feet.

- **Transmitter Receiver Test**
  - This was how we intended to control the aircraft using an RF link. Sometimes the link was usable out to a distance of as far as 150 feet; other times the link would not work with the receiver and transmitter antennas just inches apart. Even if the link was working, as soon as the transmitter and receiver began to move relative to each other, it would immediately miss a few packets of information, making it impossible to control an aircraft with the available hardware. We discovered that these ASK-style transmission devices are commonly used for short-range communications, and are not practical for RC aircraft.
