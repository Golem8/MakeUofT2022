# MakeUofT2022

**Project Name:** The Peter Tingle

**Project Description:**
Portable device that is able to tell when a friend is nearby and alert you via a vibration on your finger and a visual cue via an LED. Pairing feature allows you to register a device as a friend's device and then receive alerts when that friend comes nearby you.

**How we Built it:**
An ESP32 microcontroller with bluetooth capabilities that handles the pairing of the friend's device and detection of said device via bluetooth discovery. An Arduino that acts as a voltage regulator to the ESP32, and output to the transistor acting as a switch. Arduino also handles power delivery to the motor for the vibration generator, while the ESP handles the LED power delivery.

**Challenges:**
The Bluetooth Serial Library for Arduino was poorly documented, making the bluetooth detection and pairing functionalities of our design difficult to implement. The ESP32 microcontroller's documentation and pinouts were unclear, causing confusion during the hardware portion. Bluetooth technology in of itself is limited due to privacy concerns, meaning sensing capabilities are limited to devices either in discovery mode or previously paired devices, forcing our solution space to be fairly narrow. We had to get creative and be as resourceful as possible, using second-hand items and fashioning cardboard to build our wearable prototype. Lack of a 3D printer or access to a hardware store during the main crunch time of our project meant additional creativity was required to successfully implement our prototype.

**Contributions:**

Muaz:
Hardware Specialist.
Worked with soldering, breadboarding, hardware troubleshooting using a DMM and reviewing hardware specifications of the ESP32.

Justin:
Prototyping and Resource Management Specialist.
Worked with resource collection, soldering, transistor implementation and troubleshooting assembly of the physical prototype. 

Rohan:
Database Management and Arduino Logic Software Specialist.
Worked with interfacing between the bluetooth inputs and internal storage of the device. Implemented the logic used to control the LEDs and vibration generator.

Simon: 
Bluetooth Communications Software Specialist.
Worked with the Bluetooth Serial Library to process the raw bluetooth inputs from the ESP32 into clean data. Implemented friend's device pairing functionality.


**What's Next:**
Creating an smartphone application so that the friends feature does not require going to the bluetooth settings page. Also so that alerts can be sent from the device to the user's phone.
Use custom PCB to fit device into a wearable ring.
Add IOS support.


