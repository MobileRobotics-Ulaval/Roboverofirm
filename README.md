# Modified Robovero Firmware

#### Addition to the original firmware

- IMU driver based on https://github.com/agottem/robovero_sensors
- Second I2C bus for external devices
- Drivers for sonar SRF08, boussole CMPS03, LedarOne, ESC BLCtr1.2
- Addition of a watchdog for USB that take too much time

#### Installation

To flash the firmware follow the instruction on the [Robovero website](http://robovero.org/tutorials/firmware/). We had trouble to flash on some version of Ubuntu. The solution is install crosstool-NG version 1.20.0, instead of the version in the Robovero's tutorial. That version of  crosstool-NG can be found [here](http://crosstool-ng.org/download/crosstool-ng/).
