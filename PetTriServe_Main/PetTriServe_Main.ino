/*
  Project: PetTriServe Main Controller
  Based on: Random Nerd Tutorials (Rui Santos & Sara Santos)
  
  Hardware:
  - ESP32-2432S028R (CYD) or ESP32 Dev Module + ILI9341 TFT

  Board Settings (Arduino IDE):
  - Board: "ESP32 Dev Module"
  - Port: "COM6"
  - CPU Frequency: "240MHz (WiFi/BT)"
  - Core Debug Level: "None"
  - Erase All Flash Before Sketch Upload: "Disabled"
  - Events Run On: "Core 1"
  - Flash Frequency: "80MHz"
  - Flash Mode: "QIO"
  - Flash Size: "4MB (32Mb)"
  - JTAG Adapter: "Disabled"
  - Arduino Runs On: "Core 1"
  - Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
  - PSRAM: "Disabled"
  - Upload Speed: "921600"
  - Zigbee Mode: "Disabled"

  Libraries:
  - LVGL (v9.2) by kisvegabor (requires custom lv_conf.h)
  - TFT_eSPI by Bodmer (requires custom User_Setup.h)
  - XPT2046_Touchscreen by Paul Stoffregen
*/

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include "time.h"
#include "Portal32.h" // Include Portal32

// ==========================================
// USER CONFIGURATION
// ==========================================

// NTP Configuration
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800; // GMT+8
const int   daylightOffset_sec = 0;

// ==========================================
// HARDWARE PINS
// ==========================================

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 6 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
lv_obj_t * time_label; 

// Log callback for LVGL
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// Touchscreen Read Callback
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    
    // Calibrate Touchscreen points
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// Timer callback to update the clock
static void update_clock_cb(lv_timer_t * timer) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    // Check connection status
    if (portal.isWifiConnecting()) {
         String msg = "Connecting to:\n" + portal.getSSID();
         lv_label_set_text(time_label, msg.c_str());
    } else if (portal.isAPModeActive()) {
         lv_label_set_text(time_label, "Setup WiFi: \nConnect to 'Portal32'");
    } else if (portal.isConnected()) {
         lv_label_set_text(time_label, "Syncing Time...");
    } else {
         lv_label_set_text(time_label, "Not Connected");
    }
    return;
  }
  char timeString[9];
  // Format as HH:MM:SS
  strftime(timeString, 9, "%H:%M:%S", &timeinfo);
  lv_label_set_text(time_label, timeString);
  
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0); 
}

void lv_create_main_gui(void) {
  // Time Label
  time_label = lv_label_create(lv_screen_active());
  lv_label_set_text(time_label, "Starting...");
  
  lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

  // Create a timer to update the label every 1000ms (1 second)
  lv_timer_create(update_clock_cb, 1000, NULL);
}

void setup() {
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);
  
  // Start LVGL
  lv_init();
  lv_log_register_print_cb(log_print); // Register print function for debugging

  // Start Touchscreen SPI
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(2); // Set Touchscreen rotation

  // Initialize TFT Display
  lv_display_t * disp;
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);
    
  // Initialize Touchscreen Input Device
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Initialize Portal32
  portal.begin();
  
  // Create GUI
  lv_create_main_gui();

  // Configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  portal.handle();    // Handle Portal32 requests (DNS, WebServer, Reconnection)
  lv_task_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);           // let this time pass
}