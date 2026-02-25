# PetTriServe

## OVERVIEW

**PetTriServe** is an ESP32-based smart pet feeding system designed to automate feeding schedules for up to three separate feeders. It features a modern touchscreen interface (LVGL) for monitoring status and a built-in captive portal (Portal32) for easy WiFi and schedule configuration.

The system ensures your pets are fed on time, every time, by synchronizing with internet time servers (NTP) and executing precise feeding durations.

## FEATURES

- **Automated Feeding Control**: Independently configure up to 3 feeders.
  - Set **Start Time** (HH:MM).
  - Set **Feeding Interval** (e.g., every 4 hours) or Once Daily.
  - Set **Dispense Duration** (in seconds) for portion control.
- **Smart Touch Interface**:
  - Real-time Clock (NTP Synced).
  - Next Feeding Countdown per feeder.
  - WiFi Connection Status (SSID & IP).
  - Color-coded feeder cards (Red, Green, Blue).
- **Easy Configuration Portal (Portal32)**:
  - Connect to the device via WiFi hotspot ("Portal32").
  - Configure WiFi credentials.
  - Adjust feeding schedules and durations via a web interface.
- **Reliable Hardware Control**: Direct servo motor control for dispensing mechanisms.

## HARDWARE REQUIREMENTS

- **Microcontroller**: ESP32-2432S028R (CYD - "Cheap Yellow Display") or ESP32 Dev Module + ILI9341 TFT + XPT2046 Touch.
- **Display**: 2.8" TFT (240x320) with Touch.
- **Actuators**: 3x Servo Motors (PWM control) for dispensing food.
- **Power Supply**: 5V USB or external power suitable for ESP32 and servos.

## SOFTWARE & LIBRARIES

The project is built using the Arduino IDE and relies on the following key libraries:

1.  **LVGL** (v9.2) - Light and Versatile Graphics Library for the UI.
2.  **TFT_eSPI** (Bodmer) - High-performance display driver.
3.  **XPT2046_Touchscreen** (Paul Stoffregen) - Touch controller driver.
4.  **WiFi & WebServer** - Core ESP32 libraries for connectivity.
5.  **Portal32** - Custom included library for Captive Portal and Settings Management.

## INSTALLATION & SETUP

### 1. Prerequisites

- Install **Arduino IDE**.
- Install the required libraries via the Library Manager.
  - `LVGL`
  - `TFT_eSPI`
  - `XPT2046_Touchscreen`

### 2. Board Configuration (Arduino IDE)

- **Board**: ESP32 Dev Module
- **Flash Size**: 4MB (32Mb)
- **Partition Scheme**: Huge APP (3MB No OTA/1MB SPIFFS)
- **PSRAM**: Disabled (for standard CYD)
- **Core**: Core 1 for Events and Arduino

### 3. Display Configuration

- Ensure your `User_Setup.h` in the `TFT_eSPI` library is configured for your specific display driver (typically ILI9341 for CYD) and pinout.
- Ensure `lv_conf.h` is properly set up in the library folder or project directory.

### 4. Uploading

1.  Connect your ESP32 to the PC.
2.  Select the correct COM port.
3.  Click **Upload**.

## USAGE

### Initial Setup (WiFi & Schedule)

1.  Power on the device.
2.  If not connected to WiFi, the screen will prompt you to connect to the Access Point.
3.  On your phone/PC, connect to the WiFi network named **Portal32**.
4.  A captive portal should open (or navigate to `192.168.4.1`).
5.  **Configure WiFi**: Enter your home WiFi SSID and Password.
6.  **Configure Feeders**: Set the Start Time, Interval, and Duration for Feeder 1, 2, and 3.
7.  Save and Reboot.

### Device Operation

- The device will connect to your WiFi and sync time with NTP (GMT+8).
- The main screen displays the current time and the next scheduled feeding for each feeder.
- When a feeding time is reached:
  - The servo for that feeder will activate (move to 90°, wait for duration, then return to 180/0°).
  - The system logs the event.

## DIRECTORY STRUCTURE

- `/PetTriServe_Main`: Main Arduino sketch folder.
  - `PetTriServe_Main.ino`: Core logic, UI setup, and loop.
  - `Portal32.cpp/h`: Helper class for handling WiFi, Web Server, and Preferences.
  - `lv_conf.h`: Local LVGL configuration file.
- `/Portal32`: (Backup/Source) Portal32 library files.
- `/Servo_Controller`: (Optional) Separate module for servo testing or control.
