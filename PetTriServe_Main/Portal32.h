#ifndef PORTAL32_H
#define PORTAL32_H

#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
using namespace fs;
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

struct FeederConfig {
    int id; // 1, 2, 3
    String startTime; // HH:MM
    String interval;  // HH:MM
    int dispenseDuration; // Seconds
};

class Portal32 {
public:
    Portal32();
    void begin();
    void handle();
    bool isConnected();
    bool isWifiConnecting();
    bool isAPModeActive();
    String getSSID();
    String getIP();
    String getDeviceName();
    FeederConfig getFeeder(int id);

private:
    WebServer server;
    DNSServer dnsServer;
    Preferences preferences;

    String ssid;
    String password;
    String device_name;
    FeederConfig feeders[3]; // For Feeder 1, 2, 3
    
    bool isAPMode;
    bool isConnecting;
    bool shouldConnect;
    unsigned long connectionStartTime;
    unsigned long lastStatusCheck;

    const char* AP_SSID = "Portal32";
    const byte DNS_PORT = 53;
    IPAddress apIP;

    void loadSettings();
    void saveSettings(String s, String p, String n);
    void setupAP();
    void startSTA();
    void initWebServer();

    // Web Handlers
    void handleRoot();
    void handleScan();
    void handleSave();
    void handleStatus();
    void handleNotFound();
};

extern Portal32 portal;

#endif
