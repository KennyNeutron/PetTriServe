# PetTriServe Technical Documentation

This document provides an in-depth explanation of the **PetTriServe** smart pet feeding system's architecture, technical inner workings, and building process.

## 1. System Architecture

The project consists of two distinct processing modules communicating over standard serial (UART):

1.  **Main System Controller (ESP32-2432S028R / CYD - Cheap Yellow Display)**
    - **Role**: Handles all high-level logic, user interface, scheduling, internet connectivity, and configuration portaling.
    - **Firmware**: Located in `/PetTriServe_Main/`.
2.  **Servo Controller (Auxiliary Microcontroller, e.g., Arduino Uno/Nano)**
    - **Role**: Dedicated to precise PWM (Pulse Width Modulation) signaling for 3 independent servo motors that dispense food.
    - **Firmware**: Located in `/Servo_Controller/`.

_Note: The choice to use an auxiliary controller offloads strict PWM timing for multiple servos from the main ESP32 thread, preventing jitter while the ESP32 handles heavy Wi-Fi and UI rendering._

---

## 2. Main System Controller (ESP32)

### 2.1 Core Frameworks & Libraries

- **LVGL (v9.2)**: "Light and Versatile Graphics Library." The entire touch UI is built on this framework. It handles the rendering of text, layouts, backgrounds, and styling. Requires `lv_conf.h` in the same directory for global customization (memory allocation, font sizes, color depth).
- **TFT_eSPI**: A highly optimized display driver pushing SPI at 80MHz to draw the TFT screen quickly. Requires customized `User_Setup.h`.
- **XPT2046_Touchscreen**: Driver for reading XY coordinates from the resistive touch film on top of the CYD TFT matrix.
- **Portal32** _(Custom Component)_: A captive portal library utilizing `DNSServer`, `WebServer`, and `Preferences` APIs out-of-the-box.

### 2.2 Task Distribution via Dual Cores

The ESP32 is a dual-core SoC (System on Chip). This project configures core priorities carefully:

- **Core 1 (Application/Events)**: `setup()`, `loop()`, Wi-Fi event handling, WebServer servicing, and LVGL UI rendering tick operations.
- **Core 0 (System/Networking)**: Background RTOS tasks, internal Wi-Fi/Bluetooth stack processes. The CYD (having 4MB flash and no optional PSRAM by default) operates best when the application logic doesn't block the network stack.

### 2.3 Time and Synchronization

- The system uses the `configTime` function pointing to `pool.ntp.org` to poll global time servers. GMT offset is hard-coded to +8 Hours (`28800` seconds).
- Until the NTP time resolves, the GUI remains locked on a "Syncing Time..." status screen to prevent the feeders from reading garbage epoch values (`04:00:00 1-Jan-1970`).
- Next-feeding time logic uses standard C `<time.h>` and `struct tm` functions. Every time a second ticks, the system compares the calculated `time_t` against `mktime(&timeinfo)` to trigger an event exactly on the hour/minute limit.

### 2.4 Control Flow (Feeding Event)

1.  **Condition Met**: `isFeedingTime` becomes true in the `update_clock_cb` timer tick.
2.  **Signal Active**: The ESP32 sends a serial instruction to the Servo Controller (e.g., `S1: 90\n`) which turns the servo to 90 degrees (dispensing position).
3.  **Delay/Duration**: `delay(thisFeeder_Duration)` pauses logic. The duration is user-set via settings.
4.  **Signal Off**: The ESP32 sends the reset position string (e.g., `S1: 180\n`), stopping food flow.

---

## 3. Captive Portal (Portal32)

When no saved network can be reached, the device spins up an Access Point (`Portal32`).

- **DNS Redirection**: The `DNSServer` intercepts any `A` records from connected devices and replies dynamically to load its own IP address (`192.168.4.1`). Thus, any URL typed into the phone will launch the setup page.
- **Non-Volatile Storage (NVS)**: User details (SSID, pass, device name, and 3x feeder intervals/durations) are saved persistently to flash memory via the ESP32 `Preferences.h` API. They survive power cycles.
- **Web Server**: Handles the CSS-styled form inputs `handleRoot()`, `handleScan()`, and saves the POST requests via `handleSave()`.

---

## 4. Servo Controller Specifications

The `/Servo_Controller/Servo_Controller.ino` sketch operates fairly simply.

- **Listen**: Polls `Serial.available()`.
- **Parse**: Looking for specific `S[1-3]: [90-180]` syntax. Let's trace it: `S2: 120`. Separator `:` is found. Substring parsing identifies Servo 2, Angle 120.
- **Actuate**: `myServo2.write(angle);`.
- **Note**: There's an unresolved git conflict present natively in `Servo_Controller.ino` around lines 20-30 determining the default initialization angle (120 vs 180).

_(Important: Since both devices use Serial/TX/RX for communicating with each other, you must un-wire them when flashing the primary USB COM port to avoid upload failure)_.

---

## 5. Building & Flashing Process

### Phase A: The ESP32 Main Controller

1.  **Software Installation**: Download Arduino IDE 2.x.
2.  **Board Manager URL**: Include `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` in Board Preferences. Install `esp32`.
3.  **Download Dependencies**:
    - Go to Library Manager -> Search `lvgl` (Install v9.2.x).
    - Search `TFT_eSPI` (Install latest).
    - Search `XPT2046_Touchscreen` (Install latest).
4.  **Configure Environment**:
    - **Board**: `ESP32 Dev Module`
    - **Events/Arduino Run On**: `Core 1`
    - **Flash Size**: `4MB (32Mb)`
    - **Partition Scheme**: `Huge APP (3MB No OTA / 1MB SPIFFS)` — _Critical step: LVGL uses too much flash memory for the default 1MB APP scheme._
    - **Upload Speed**: `921600` for quicker payload transfer.
5.  **Hit Upload**: Flash `/PetTriServe_Main/PetTriServe_Main.ino` to the CYD ESP32 device.

### Phase B: The Servo Controller

1.  **Board Selection**: Switch the IDE Board selection from ESP32 to `Arduino Uno` (or Nano, depending on your physical board handling the servos).
2.  **Port**: Select the COM port specific to the auxiliary Arduino board.
3.  **Conflict Resolution**: Before compiling, open `/Servo_Controller/Servo_Controller.ino`
4.  **Hit Upload**: Flash the `/Servo_Controller/Servo_Controller.ino`.

### Phase C: Initial Setup

1.  Wire the `TX` pin of the ESP32 to the `RX` pin of the Servo Controller (and remember ground logic paths).
2.  Power on. The screen will read: `Setup WiFi: Connect to 'Portal32'`.
3.  Connect your smartphone to this hotspot.
4.  Punch in details for your local router and feeding timelines.
5.  Click `Save`. The ESP32 reboots, grabs NTP time over your real Wi-Fi network, and the dashboard will materialize.
