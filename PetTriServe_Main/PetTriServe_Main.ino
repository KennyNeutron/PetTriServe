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
lv_obj_t * date_label;
lv_obj_t * home_cont = NULL;
lv_obj_t * status_label = NULL;

time_t calculateNextFeedingTime(String startTime, String interval, struct tm &timeinfo) {
    if (startTime.length() < 5) return 0;
    
    int startH = startTime.substring(0, 2).toInt();
    int startM = startTime.substring(3, 5).toInt();
    int intH = 0, intM = 0;
    if (interval.length() >= 5) {
        intH = interval.substring(0, 2).toInt();
        intM = interval.substring(3, 5).toInt();
    }
    
    struct tm feedingTime = timeinfo;
    feedingTime.tm_hour = startH;
    feedingTime.tm_min = startM;
    feedingTime.tm_sec = 0;
    
    time_t now = mktime(&timeinfo);
    time_t feeding = mktime(&feedingTime);
    
    if (feeding == -1) return 0;

    long intervalSec = (intH * 3600) + (intM * 60);

    // If interval defines repeated feedings
    if (intervalSec > 0) {
        // Find the next feeding time in the future
        while (feeding <= now) {
            feeding += intervalSec;
        }
    } else {
        // Just once a day
        if (feeding <= now) {
            feeding += 86400; // Add one day
        }
    }
    return feeding;
}

String formatTime(time_t t) {
    struct tm * nextFeed = localtime(&t);
    char buf[6];
    sprintf(buf, "%02d:%02d", nextFeed->tm_hour, nextFeed->tm_min);
    return String(buf);
}

void create_home_screen() {
    if (home_cont != NULL) return; // Already created

    lv_obj_clean(lv_screen_active());
    home_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(home_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(home_cont, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(home_cont, 0, 0);
    lv_obj_set_flex_flow(home_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(home_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(home_cont, 5, 0);       // Reduced padding
    lv_obj_set_style_pad_row(home_cont, 2, 0);       // Reduced gap between rows

    // Time Label (Large)
    time_label = lv_label_create(home_cont);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_28, 0); // Reduced by ~20%
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(time_label, "00:00:00");

    // Date Label
    date_label = lv_label_create(home_cont);
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xCCCCCC), 0);
    lv_label_set_text(date_label, "Loading...");

    // Separator
    lv_obj_t * line = lv_line_create(home_cont);
    static lv_point_precise_t line_points[] = { {0, 0}, {200, 0} };
    lv_line_set_points(line, line_points, 2);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_color(line, lv_color_hex(0x444444), 0);
    lv_obj_set_style_margin_ver(line, 5, 0);         // Reduced margin

    // Feeder Container
    lv_obj_t * feeders_cont = lv_obj_create(home_cont);
    lv_obj_set_size(feeders_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(feeders_cont, 0, 0);
    lv_obj_set_style_border_width(feeders_cont, 0, 0);
    lv_obj_set_flex_flow(feeders_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(feeders_cont, 0, 0);    
    lv_obj_set_style_pad_row(feeders_cont, 4, 0);    

    // Feeder Colors: Red, Green, Blue
    uint32_t colors[] = { 0xCC3333, 0x33CC33, 0x3333CC }; 

    for(int i=0; i<3; i++) {
        lv_obj_t * f_card = lv_obj_create(feeders_cont);
        lv_obj_set_size(f_card, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_bg_color(f_card, lv_color_hex(colors[i]), 0);
        lv_obj_set_style_bg_opa(f_card, 150, 0); // Slight transparency
        lv_obj_set_style_pad_left(f_card, 5, 0);
        lv_obj_set_style_pad_top(f_card, 5, 0);
        lv_obj_set_style_pad_bottom(f_card, 5, 0);
        lv_obj_set_style_pad_right(f_card, 35, 0); // Increased to move time left (~10%)
        lv_obj_set_style_radius(f_card, 5, 0);
        lv_obj_set_flex_flow(f_card, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(f_card, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_border_width(f_card, 0, 0);
    }

    // Network Info Container
    lv_obj_t * net_info = lv_obj_create(home_cont);
    lv_obj_set_size(net_info, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(net_info, 0, 0);
    lv_obj_set_style_border_width(net_info, 0, 0);
    lv_obj_set_flex_flow(net_info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(net_info, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(net_info, 2, 0);
    
    // SSID Label
    lv_obj_t * ssid_lbl = lv_label_create(net_info);
    lv_label_set_text_fmt(ssid_lbl, "WiFi: %s", portal.getSSID().c_str());
    lv_obj_set_style_text_font(ssid_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ssid_lbl, lv_color_hex(0x888888), 0);

    // IP Label
    lv_obj_t * ip_lbl = lv_label_create(net_info);
    lv_label_set_text_fmt(ip_lbl, "IP: %s", portal.getIP().c_str());
    lv_obj_set_style_text_font(ip_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ip_lbl, lv_color_hex(0x888888), 0);
}

// Helper to update specific feeder card
void update_feeder_card(lv_obj_t * parent, int index, String nextTime, int duration) {
    lv_obj_t * card = lv_obj_get_child(parent, index);
    if (!card) return;
    
    // Clear existing content to refresh
    lv_obj_clean(card);
    
    // Container for Name and Duration (Left side)
    lv_obj_t * left_cont = lv_obj_create(card);
    lv_obj_set_size(left_cont, LV_PCT(55), LV_SIZE_CONTENT); // Shortened width (~ -10%)
    lv_obj_set_style_bg_opa(left_cont, 0, 0);
    lv_obj_set_style_border_width(left_cont, 0, 0);
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(left_cont, 0, 0);
    lv_obj_set_style_pad_gap(left_cont, 0, 0); // No gap

    lv_obj_t * name = lv_label_create(left_cont);
    lv_label_set_text_fmt(name, "Feeder %d (%ds)", index + 1, duration);
    lv_obj_set_style_text_font(name, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(name, lv_color_hex(0xFFFFFF), 0);

    // Time (Right side)
    lv_obj_t * time = lv_label_create(card);
    String txt = "Next: " + nextTime;
    lv_label_set_text(time, txt.c_str());
    lv_obj_set_style_text_font(time, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(time, lv_color_hex(0xFFFFFF), 0); 
    lv_obj_set_style_text_decor(time, LV_TEXT_DECOR_NONE, 0);
}

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
// Timer callback to update the clock
static void update_clock_cb(lv_timer_t * timer) {
  struct tm timeinfo;
  bool timeSynced = getLocalTime(&timeinfo);

  if(!timeSynced){
    // Not synced yet, show status
    if (home_cont != NULL) {
         lv_obj_del(home_cont);
         home_cont = NULL;
         status_label = NULL;
    }
    
    if (status_label == NULL && home_cont == NULL) {
        lv_obj_clean(lv_screen_active());
        status_label = lv_label_create(lv_screen_active());
        lv_obj_set_style_text_font(status_label, &lv_font_montserrat_20, 0); 
        lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 0);
    }
    
    if (status_label != NULL) {
        if (portal.isWifiConnecting()) {
             String msg = "Connecting to:\n" + portal.getSSID();
             lv_label_set_text(status_label, msg.c_str());
        } else if (portal.isAPModeActive()) {
             lv_label_set_text(status_label, "Setup WiFi: \nConnect to 'Portal32'");
        } else if (portal.isConnected()) {
             lv_label_set_text(status_label, "Syncing Time...");
        } else {
             lv_label_set_text(status_label, "Not Connected");
        }
    }
    return;
  }

  // Time is synced!
  if (status_label != NULL) {
       lv_obj_del(status_label);
       status_label = NULL;
  }
  
  if (home_cont == NULL) {
      create_home_screen();
  }

  // Update Time
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  lv_label_set_text(time_label, timeStr);
  
  // Update Date
  char dateStr[20];
  strftime(dateStr, sizeof(dateStr), "%a, %b %d", &timeinfo);
  lv_label_set_text(date_label, dateStr);

  // Update Feeders
  lv_obj_t * feeders_cont = lv_obj_get_child(home_cont, 3); 
  
  if (feeders_cont) {
      time_t now = mktime(&timeinfo);
      for(int i=0; i<3; i++) {
          FeederConfig f = portal.getFeeder(i+1);
          time_t nextT = calculateNextFeedingTime(f.startTime, f.interval, timeinfo);
          String nextStr = (nextT > 0) ? formatTime(nextT) : "N/A";
          
          update_feeder_card(feeders_cont, i, nextStr, f.dispenseDuration);

          // Check if it's time to feed!
          // We check if current time matches a feeding schedule.
          // Since calculateNextFeedingTime returns the *next* time (future), we need to check if we *just* passed a scheduled time.
          // A better way: Recalculate based on start time and interval to see if 'now' is a feeding time.
          
          long intervalSec = 0;
          if (f.interval.length() >= 5) {
              intervalSec = (f.interval.substring(0, 2).toInt() * 3600) + (f.interval.substring(3, 5).toInt() * 60);
          }
          
          bool isFeedingTime = false;
          
          // Parse start time to time_t for today
          struct tm startTM = timeinfo;
          startTM.tm_hour = f.startTime.substring(0, 2).toInt();
          startTM.tm_min = f.startTime.substring(3, 5).toInt();
          startTM.tm_sec = 0;
          time_t startT = mktime(&startTM);
          
          if (intervalSec > 0) {
              // Repeated feeding
              // If now >= startT and (now - startT) % interval == 0
              if (now >= startT) {
                  long diff = now - startT;
                  if (diff % intervalSec == 0 && timeinfo.tm_sec == 0) {
                      isFeedingTime = true;
                  }
              }
          } else {
              // Once a day
              if (now == startT) {
                   isFeedingTime = true;
              }
          }
          
          if (isFeedingTime) {
             Serial.printf("Feeding Time for Feeder %d!\n", i+1);
             // Convert duration (seconds) to milliseconds for delay
             Feeder_Dispense(i+1, f.dispenseDuration * 1000); 
          }
      }
  }
}

// Feeder Dispense Function
void Feeder_Dispense(uint8_t thisFeeder, uint16_t thisFeeder_Duration){
    Serial.println("S"+String(thisFeeder)+": 90");
    delay(thisFeeder_Duration);
    Serial.println("S"+String(thisFeeder)+": 180");
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
  
  // Start Query Timer
  lv_timer_create(update_clock_cb, 1000, NULL);

  // Configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  portal.handle();    // Handle Portal32 requests (DNS, WebServer, Reconnection)
  lv_task_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);           // let this time pass
}


