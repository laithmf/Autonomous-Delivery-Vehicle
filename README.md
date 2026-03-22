# 🚗 Autonomous Delivery Vehicle

**Wayne State University — ET 4999 Senior Project | Fall 2023**  
**Authors:** Laith Fakhruldin & Spencer Martinez  
**Advisor:** Professors Dr. Ece Yaprak & Moise Sunda  
**Degree:** Bachelor of Science in Computer Technology

---

## 📌 Project Overview

An autonomous miniature delivery vehicle designed for warehouse environments. The vehicle follows a black-line track, detects color-coded pickup and drop-off stations using an RGB sensor, picks up and delivers items using a 3D-printed robotic arm, and avoids obstacles in real time and all controlled wirelessly via Bluetooth from a Python desktop application.

This project demonstrates how sensor-driven automation can replace repetitive human labor in controlled industrial settings, improving efficiency and reducing error margins.

---

## 🎯 Key Features

- **Line Tracking** — TCRT5000 IR sensors keep the vehicle on a fixed black track
- **Color Recognition** — TCS34725 RGB sensor identifies orange, blue, and purple stations
- **Obstacle Avoidance** — HC-SR04 ultrasonic sensor detects and reroutes around obstacles
- **Robotic Arm** — 3-servo arm (claw, elbow, base) picks up and drops off items at correct stations
- **Bluetooth Control** — HC-05 module links the Arduino to a Python GUI over serial
- **Python GUI** — Tkinter desktop app to select stations, send commands, monitor status
- **9 Delivery Modes** — All combinations of Orange / Blue / Purple pick-up and drop-off
- **3-Cycle Simulation** — Automated sequence: Orange → Blue → Purple stations

---

## 🛠️ Hardware Components

| Component | Role |
|---|---|
| LAFVIN Smart Car Robot (Arduino Mega) | Main chassis and microcontroller |
| TCS34725 RGB Color Sensor | Detects station color for pick/drop logic |
| HC-SR04 Ultrasonic Sensors (×2) | Item detection + obstacle avoidance |
| TCRT5000 IR Sensors (×3) | Black-line tracking (left, center, right) |
| PCA9685 Servo Motor Driver | Controls 3 servo motors on the robotic arm |
| HC-05 Bluetooth Module | Wireless serial communication with Python |
| KY-034 7-Color LED | Visual alarm when an obstacle is detected |
| CD74HC4067 & TCA9548A Multiplexers | Expand available I/O pins on the Arduino |
| 3D-Printed Parts (TinkerCAD) | Custom claw and arm components |

---

## 💻 Software Stack

| Layer | Technology |
|---|---|
| Microcontroller firmware | C++ (Arduino IDE) |
| Desktop control app | Python 3 + Tkinter |
| Bluetooth serial communication | `pyserial` library |
| Servo driver | Adafruit PCA9685 library |
| Color sensor | Adafruit TCS34725 library |
| Ultrasonic ranging | NewPing library |

---

## 📁 Repository Structure

```
autonomous-delivery-vehicle/
│
├── README.md                                    # This file
├── Senior_Project_Delivery_Vehicle_12_1_23.ino  # Arduino firmware (C++)
├── AutonomousDeliveryVehicle.py                 # Python GUI control app
└── Autonomous_Delivery_Vehicle_Report.docx      # Full academic report
```

---

## ⚙️ How It Works

### System Architecture

```
Python GUI (PC)
     │
     │  Bluetooth (HC-05)
     ▼
Arduino Mega
     ├── TCS34725 RGB Sensor  →  Detects station color
     ├── TCRT5000 IR Sensors  →  Follows black line track
     ├── HC-SR04 (front)      →  Obstacle avoidance
     ├── HC-SR04 (side)       →  Item proximity detection
     └── PCA9685 Driver       →  Controls 3-servo robotic arm
```

### Delivery Cycle (example: Orange → Blue)
1. Python app sends command `'b'` over Bluetooth
2. Vehicle follows the black line track
3. RGB sensor detects **orange** station → arm picks up item
4. Vehicle continues on track; timer prevents premature drop-off
5. RGB sensor detects **blue** station → arm drops item
6. Vehicle reaches red finish line → stops and resets

### Command Reference

| Python Key | Action |
|---|---|
| `o` | Orange → Orange |
| `b` | Orange → Blue |
| `p` | Orange → Purple |
| `e` | Blue → Blue |
| `l` | Blue → Orange |
| `m` | Blue → Purple |
| `a` | Purple → Purple |
| `c` | Purple → Orange |
| `d` | Purple → Blue |
| `i` | Run 3-cycle simulation |
| `s` | Emergency stop |
| `n` | Continue after stop |
| `g` | Full reset |

---

## 🚀 Getting Started

### Prerequisites

**Arduino side:**
- [Arduino IDE](https://www.arduino.cc/en/software)
- Libraries (install via Library Manager):
  - `Adafruit TCS34725`
  - `Adafruit PWM Servo Driver`
  - `NewPing`

**Python side:**
- Python 3.x
- Install dependencies:
  ```bash
  pip install pyserial
  ```
  *(tkinter is included with standard Python on Windows)*

### Setup & Run

1. **Upload Arduino firmware**
   - Open `Senior_Project_Delivery_Vehicle_12_1_23.ino` in Arduino IDE
   - Select your board (Arduino Mega) and correct COM port
   - Upload the sketch

2. **Pair the HC-05 Bluetooth module**
   - Default pairing PIN: `1234`
   - Note the assigned COM port (e.g., `COM6`) after pairing on Windows

3. **Run the Python app**
   ```bash
   python AutonomousDeliveryVehicle.py
   ```

4. **Connect via GUI**
   - Click **Connect Bluetooth** → enter the COM port (e.g., `COM6`)
   - Select a Pickup Station and Drop-off Station using the radio buttons
   - Click **Send Command** to start a delivery cycle

---

## 📐 RGB Color Calibration

The TCS34725 sensor is calibrated to the physical colored mats used as stations. If you rebuild this project, you may need to re-tune the RGB threshold ranges in the `.ino` file for your environment:

```cpp
// Example: Orange detection thresholds
bool isOrange(unsigned int red, unsigned int green, unsigned int blue) {
  int redMin = 125; int redMax = 255;
  int greenMin = 71; int greenMax = 145;
  int blueMin = 55;  int blueMax = 95;
  ...
}
```
Use the serial monitor in Arduino IDE with the commented-out debug print statements to read raw sensor values for your specific lighting and mat colors.

---

## 📊 Engineering Standards

This project was developed in compliance with **ISO 9001:2015**, ensuring:
- Customer requirements are met (successful item transport with package integrity)
- Continuous process improvement through iterative testing and design revisions
- Documented test cases and performance evaluations

---

## ⚠️ Known Limitations & Future Improvements

- Same-color pick/drop routing uses a time delay workaround (a second sensor would be more robust)
- The vehicle operates on a fixed, pre-defined track — dynamic path planning is not implemented
- Bluetooth range is limited (~10m); Wi-Fi or MQTT would scale better
- A camera or LIDAR sensor could replace the RGB mat system for more flexibility

---

## 📄 License

Copyright © 2023 Laith Fakhruldin & Spencer Martinez. All rights reserved.  
This project was submitted in partial fulfillment of the requirements for the degree of Bachelor of Science in Computer Technology at Wayne State University.
