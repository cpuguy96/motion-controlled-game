# motion-controlled-game

Final Project for Embedded Systems Design

Video demonstration: https://youtu.be/oSO7fs6h6BM

## Prerequisites

The console code is based off the [EK-TM4C123GXL](http://www.ti.com/tool/EK-TM4C123GXL) from Texas Instruments. The I/O mappings must be changed in order to run the code. The microcontroller must also drive at least 100mA for the ESP8266 to function correctly.

[Kiel uVision4 IDE](https://www.keil.com/demo/eval/armv4.htm) was used to program and build the console code. Kiel Version 5 might work, but it is not tested. The demo version can only support building up a 56Kb size project. You can find the instructions to install Kiel uVision [here](http://users.ece.utexas.edu/~valvano/EE445L/).

The controllers were programmed with the ESP8266 ESP-12E borads using the [Arduino IDE](https://www.arduino.cc/).

We used [Eclipse](https://www.eclipse.org/) to compile and run the game display, but any Java IDE will suffice.

## Installing

Create a uVision project in the console folder and configure it for the TM4C123GH6PM processor. Include all C and assmbley files in the project itmes. Build and flash the project onto the TM4C microcontroller.

Add the ESP8266 library to the Arduino IDE and set the board to ESP-12E Module. Flash the controller.ino files to the ESP-12E.

Using Eclipse, add Slick2D, LWJGL3, and jSerialComm to the Java Build Path. The display will only run when the TM4C serial communication port is detected, so make sure the TM4C is connected to the computer before running.

## Built With

* [jSerialComm](https://github.com/Fazecast/jSerialComm) - Used for serial communication between computer and microcontroller
* [Slick2D](http://slick.ninjacave.com/) - Used for game display
* [LWJGL3](https://www.lwjgl.org/) - Dependency for Slick2D
* [Tiled](https://www.mapeditor.org/) - Used to create game maps

## Authors

* **Chimeize Iwuanyanwu** - *Console circuit design, wireless/serial communication, game model*
* **Cole Thompson** - *Controller circuit design, motion controls, game model and display* 


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* The Nindendo Wii for inspiration for the project
* [Sebastian Madgwick](http://x-io.co.uk/open-source-imu-and-ahrs-algorithms/) for his IMU and AHRS algorithms

