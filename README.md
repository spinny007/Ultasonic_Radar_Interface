# Rudra R360° V1 — Dual-Array Advanced Radar Interface

An advanced, 360-degree real-time tactical radar scanning system built on the ESP8266 platform. By pairing a servo motor with two back-to-back HC-SR04 ultrasonic sensors, this architecture achieves true panoramic detection coverage with zero blind spots while managing a responsive web configuration suite and telemetry dashboard over WebSockets.

---

## 🛰️ Project Overview

Traditional ultrasonic radars sweep a 180-degree field of view, wasting precious sweep time reversing direction. The **Rudra R360°** utilizes a **dual-array layout (Forward Array S1 and Aft Array S2)** separated by 180 degrees on a single SG90 servo motor shaft. 

As the motor sweeps smoothly between 0° and 179°, the combined data stream paints a comprehensive 360-degree tactical picture across an immersive, high-refresh HTML5 canvas interface.

---

## 🛠️ Key Architectural Features

* **True 360° Scanning:** Back-to-back hardware design means S1 covers 0°–179° while S2 simultaneously maps 180°–359°.
* **Persistent Web UI & Telemetry:** Embedded real-time dashboard powered by persistent raw HTML/JS buffers (`PROGMEM`) pushed over ultra-low-latency WebSockets.
* **Dual Network Access Modes:**
  * **Access Point (AP) Mode:** Generates its own reliable Wi-Fi network (`RadarNode-001`) out-of-the-box.
  * **Station (STA) Mode:** Connects cleanly to your existing local home or industrial Wi-Fi network.
* **Fail-Safe Captive Portal Integration:** Automatically hooks all HTTP requests via an internal DNS server if your local network connection falls over for more than 60 seconds.
* **Hardware Emulation Mode:** Built-in dashboard simulator lets you evaluate target tracking filters, heatmaps, and coordinate arrays without physical components wired up.
* **On-Board Sector Isolation:** Dynamic web interface filters allow you to isolate precise scan angles or mute designated blind zones on the fly.

---

## 📦 System Architecture & File Layout

* **`radar.ino`**: Core firmware codebase managing sensor telemetry, servo articulation, persistent EEPROM storage routines, Wi-Fi connectivity states, and HTTP route routing.
* **`radar_index_html.h`**: Encapsulated UI layout housing two distinct operator dashboards:
  1. **Tactical Console (`index_html`)**: The primary operator dashboard featuring canvas plotting, historical obstacle classification logs, and a 360-degree visual tracking activity heatmap.
  2. **Config Panel (`config_html`)**: An isolated infrastructure configuration portal protected by master-hardcoded authorization credentials.

---

## 🔌 Hardware Configuration

### Component Requirements
* **Microcontroller:** ESP8266 (NodeMCU, WeMos D1 Mini, etc.)
* **Servo Motor:** 1x SG90 or MG90S 180° Servo
* **Ultrasonic Transceivers:** 2x HC-SR04 Modules
* **Power Supply:** External 5V rail (highly recommended to avoid ESP8266 voltage sags during motor swings)

### Pinout Mapping Table

| Component Component | Component Pin | ESP8266 GPIO Pin |
| :--- | :--- | :--- |
| **Servo Motor** | Signal Pin | `D4` (GPIO2) |
| **Ultrasonic S1 (Forward)** | Trigger Pin | `D1` (GPIO5) |
| **Ultrasonic S1 (Forward)** | Echo Pin | `D2` (GPIO4) |
| **Ultrasonic S2 (Aft)** | Trigger Pin | `D5` (GPIO14) |
| **Ultrasonic S2 (Aft)** | Echo Pin | `D6` (GPIO12) |

---

## 🚀 Step-by-Step Deployment

### 1. Software Configuration
1. Open the repository directory within your **Arduino IDE** or **VS Code + PlatformIO**.
2. Install the necessary system dependencies using your IDE Library Manager:
   * `WebSockets` (by Markus Sattler)
3. Connect your ESP8266 to your workstation via USB. Ensure the proper board profile and COM port are selected.
4. Flash the code directly to your hardware chip.

### 2. Accessing the Console Interface
1. Power up the system. Search for available Wi-Fi networks on your mobile device or computer and connect to the system access point:
   * **Default SSID:** `RadarNode-001`
   * **Default Wi-Fi Passkey:** `radar@secure99`
2. Open your preferred browser and navigate to the root dashboard tracking link:
   * **URL:** `http://192.168.4.1/`
   * **Default Web UI Credentials:** `admin` / `radar1234`

### 3. Setting Up Home/Office Wi-Fi Routing
1. To seamlessly link the hardware system onto your standard network infrastructure, navigate to the config portal route:
   * **URL:** `http://192.168.4.1/config`
   * **Master Config Credentials:** `superadmin` / `cfg@Master77`
2. Select **Discover** to pulse your local Wi-Fi environment, select your wireless access point, and provide its authorization password key.
3. Choose **Execute Cold Reboot** to save the network details straight to the device's storage and connect.

---

## 🔒 Security Notice
> [!WARNING]  
> The administrative configurations system uses unchangeable master credential constants (`superadmin` / `cfg@Master77`) directly inside the `radar.ino` firmware. It is highly recommended to update these hardcoded compiler directives manually inside production environments prior to flashing the system onto field nodes.

---

## 📜 License
This software and its companion hardware layout definitions are distributed under standard open-source parameters. Feel free to modify, expand, or integrate this project into your customized sensory exploration platforms.