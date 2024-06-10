# TinyML Gesture-Controlled Car

## Project Overview
This project demonstrates the implementation of a gesture-controlled robotic car using TinyML on ESP32 microcontrollers. It leverages machine learning at the edge to interpret hand gestures, allowing the car to be controlled without the need for connectivity to a server or cloud.

## Features
- **Gesture Control**: Control the car's movement through physical gestures.
- **Real-Time Response**: Utilizes the ESP32's capability for real-time data processing.
- **Local Processing**: All processing is done on the device, ensuring privacy and responsiveness.

## Hardware Requirements
- 2 x ESP32 WROOM-DA Dev Kits
- Generic robotic car kit
- Motor Driver (e.g., SparkFun Dual TB6612FNG)
- MPU6050 gyroscope and accelerometer
- Miscellaneous: push buttons, LEDs, battery pack, breadboard, jumper wires

## Software Requirements
- Arduino IDE
- Libraries:
  - ESP32 Websockets
  - TensorFlow Lite Micro (tflm_esp32)
  - Other libraries for interfacing with hardware components

## Installation
1. **Set up the Arduino environment**:
   - Install the Arduino IDE.
   - Configure the ESP32 boards in the Arduino IDE.

2. **Clone the repository**:
   ```bash
   git clone https://github.com/nickdebaise/TinyML_Gesture-Controlled_Car.git

## Motivation
The project was built By Austin Gregory and Nick DeBaise for ECE-328: Internet of Things, taught by Professor Okwori, at Union College.

