# Intrusion & Flame Detection Security System

![ESP32](https://img.shields.io/badge/ESP32-Bluetooth%20Enabled-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange)
![License](https://img.shields.io/badge/License-MIT-green)
![Language](https://img.shields.io/badge/C++-blue)
![WebApp](https://img.shields.io/badge/NodeJS-green)
![WebApp](https://img.shields.io/badge/ReactJS-lightblue)

A **low-cost, dual-hazard security system** built around an ESP32 microcontroller. It detects **flames** and **door opening events** (intrusion) using IR-based sensors and communicates **system status** (`ARMED`, `DISARMED`, `RESET`, `HELP`) to your smartphone via Bluetooth.

---

## 🚨 Features

- 🔥 **Flame Detection** – IR flame sensor detects fire hazards (760nm–1100nm)
- 🚪 **Door/Intrusion Detection** – MH Flying Fish IR sensor detects when a **door (object) is removed** from its field of view
- 🔊 **Audible & Visual Alerts** – Buzzer and RGB LED provide immediate local alerts
- 📱 **Bluetooth Command Interface** – Send commands (`HELP`, `ARM`, `DISARM`, `RESET`) and receive status updates
- ⚙️ **Adjustable Sensitivity** – Onboard potentiometers let you tune each sensor to your environment
- 🔋 **Low Power & Modular** – Easy to expand or embed in existing setups

---

## 🧠 System Architecture
┌─────────────────┐ ┌─────────────────────────┐ ┌─────────────────────────┐
│ Flame Sensor │ │ IR Intrusion Sensor │ │ ESP32 │
│ (LM393 based) │────▶│ (MH Flying Fish) │────▶│ - Reads sensors │
└─────────────────┘ │ - Object = Door CLOSED │ │ - Controls buzzer/LED │
│ - No object = Door OPEN│ │ - Processes commands │
└─────────────────────────┘ │ - Sends status msgs │
└───────────┬─────────────┘
│ Bluetooth (SPP)
▼
┌─────────────────────────┐
│ Smartphone │
│ - Send ARM/DISARM/etc │
│ - Receive status │
└─────────────────────────┘


---

## 🛠️ Hardware Components

| Component                     | Purpose                              |
|-------------------------------|--------------------------------------|
| ESP32 Development Board       | Main controller + Bluetooth          |
| Flame Sensor Module (LM393)   | Detects IR radiation from flames     |
| MH Flying Fish IR Sensor      | Detects door open (object removed)   |
| RGB LED (common cathode)      | Visual status indicator              |
| Buzzer Module (Keyes)         | Audible alarm                        |
| 5V Power Supply / USB Cable   | System power                         |

---

## 🔌 Pin Mapping (Example)

| Component              | ESP32 Pin |
|------------------------|-----------|
| Flame Sensor (DO)      | GPIO 13   |
| IR Intrusion (OUT)     | GPIO 12   |
| RGB LED (Red)          | GPIO 14   |
| RGB LED (Green)        | GPIO 27   |
| RGB LED (Blue)         | GPIO 26   |
| Buzzer                 | GPIO 15   |

*Adjust pins as needed in the code.*

---

## 📡 Bluetooth Commands & Status Messages

The ESP32 communicates over **Bluetooth Serial (SPP)** at **9600 baud**.

### 🔹 Commands you can send from your phone:

| Command     | Action                                                                 |
|-------------|------------------------------------------------------------------------|
| `HELP`      | System replies with a summary of all available commands               |
| `ARM`       | Arms the system – intrusion and flame monitoring is active            |
| `DISARM`    | Disarms the system – no alerts will be triggered                      |
| `RESET`     | Resets the system (clears alerts, returns to last armed/disarmed state)|

### 🔹 Status messages sent by the ESP32:

| Message                     | Meaning                                      |
|-----------------------------|----------------------------------------------|
| `SYSTEM ARMED`              | System is active and monitoring              |
| `SYSTEM DISARMED`           | System is inactive                           |
| `FLAME DETECTED!`           | Fire hazard detected                         |
| `INTRUSION DETECTED!`       | Door was opened (object removed from sensor) |
| `SYSTEM RESET`              | Reset command acknowledged                   |
| `HELP - Commands: ARM, DISARM, RESET, HELP` | Command summary |

---

## 🚀 Getting Started

### 1. Clone the repository
```bash
git clone https://github.com/yourusername/intrusion-flame-security.git
cd intrusion-flame-security

###2
