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

```

### 2. Install PlatformIO (recommended) or Arduino IDE
•PlatformIO – VS Code extension

•Or Arduino IDE with ESP32 board support


### 3. Upload the code
Open the project and upload the main sketch to your ESP32.

### 4. Pair your smartphone
  •Enable Bluetoooth on your phone
  
  •Pair with ESP32_Security (or custom name)
  
  •Open a blueetooth serial app and connect

### 5. Test the system

  •Send HELP – you should receive the command list
  
  •Send ARM – system starts monitoring
  
  •🔥 Flame test – bring a lighter near the flame sensor → buzzer + LED + alert
  
  •🚪 Door test – place an object in front of the IR sensor (door CLOSED), then remove it (door OPEN) → intrusion alert
  
  •Send DISARM – system stops alerts
  
  •Send RESET – clears any active alert state
  


## 🔐 How Intrusion Detection Works

  •The MH Flying Fish IR sensor continuously emits IR light and looks for reflection.
  
  •When a door is closed, an object (e.g., cardboard, wood panel) is placed directly in front of the sensor → IR reflects back → sensor reads LOW (door present).
  
  •When the door is opened, the object is removed → no IR reflection → sensor reads HIGH → intrusion triggered.
  
  •This mimics a real door monitoring system: if the door is opened without disarming, an alert is raised.

## 🧪 Lessons Learned

  •Door simulation: Using a physical object to represent a closed door works reliably, but sensor positioning is critical.
  
  •Sensor tuning – potentiometers must be adjusted to detect presence/absence without false triggers.
  
  •Arming logic – alerts should only trigger when ARMED; DISARMED ignores all sensors.
  
  •Command parsing – Bluetooth input must handle line endings (\n, \r\n) correctly.
  
  •Debouncing – brief delays prevent multiple rapid intrusion events on door open.

## 🔮 Future Improvements

  •📡 Wi-Fi + Cloud alerts (Telegram, Blynk, or Firebase)
  
  •📱 Custom mobile app with ARM/DISARM buttons and alert history
  
  •🔑 PIN code authentication before disarming
  
  •📷 Camera module to capture image on intrusion
  
  •🔋 Battery backup for power outages
  
  •🧠 Multiple zones – monitor several doors with one system  

## 👥 Authors

Project Team – Intrusion & Flame Detection Security System

Built as an embedded systems / IoT security project

### Agnissan Christ Israel Robotics engineering student, Yedoh Esme Mariam Data Science and Big Data Technologies student, Zainab Yusuf Ibrahim Chemical Engineering student


Web and Mobile Apps are still in development....
