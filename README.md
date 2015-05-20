# Aerostabile
We aim to develop cubic blimps for artistic performances. They are entirely autonomous, based on IMU, sonars and USB cameras. Localization, control and interaction behaviors are developped in ROS. In the current state of our work, only 2 nodes run on the gumstix: one to handle the Robovero hardware (sonars, motors, battery level) and the other for the USB cameras (PointGrey). More information [here](http://robot.gmc.ulaval.ca/en/research/theme409.html).

## Modified Robovero Firmware

#### Addition to the original firmware

- IMU driver based on https://github.com/agottem/robovero_sensors, with gyroscope buffering.
- Second I2C bus for external devices
- Drivers for sonar SRF08, compass CMPS03, [LeddarOne](http://leddartech.com/en/products-sensors/leddar-one-module), [ESC BL-Ctrl v1.2](http://wiki.mikrokopter.de/en/BL-Ctrl_V1.2)
- Addition of a watchdog reseting the board if I2C hangs

#### Installation

To flash the firmware follow the instruction on the [Robovero website](http://robovero.org/tutorials/firmware/). We had trouble to flash on some version of Ubuntu. For us, using crosstool-NG version 1.20.0, instead of the version in the Robovero's tutorial worked. That version of crosstool-NG can be found [here](http://crosstool-ng.org/download/crosstool-ng/).

###### Acknowledgement
This research is conducted by the Mobile Robotics laboratory of professor Philippe Giguere at University Laval, with the collaboration of the research groups of professor Inna Sharf and professor Gregory Dudek at McGill University and under the artistic direction of Nicolas Reeves, professor at UQAM. All the development is coordinated by David St-Onge, eng. We would like to thanks the FQRNT-Team grant for their support.
