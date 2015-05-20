# Modified Robovero Firmware

#### Addition to the original firmware

- IMU driver based on https://github.com/agottem/robovero_sensors
- Second I2C bus for external devices
- Drivers for sonar SRF08, compass CMPS03, [LeddarOne](http://leddartech.com/en/products-sensors/leddar-one-module), [ESC BL-Ctrl v1.2](http://wiki.mikrokopter.de/en/BL-Ctrl_V1.2)
- Addition of a watchdog reseting the board if I2C hangs

#### Installation

To flash the firmware follow the instruction on the [Robovero website](http://robovero.org/tutorials/firmware/). We had trouble to flash on some version of Ubuntu. For us, using crosstool-NG version 1.20.0, instead of the version in the Robovero's tutorial worked. That version of crosstool-NG can be found [here](http://crosstool-ng.org/download/crosstool-ng/).
