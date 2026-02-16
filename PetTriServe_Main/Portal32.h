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

private:
    WebServer server;
    DNSServer dnsServer;
    Preferences preferences;

    String ssid;
    String password;
    String device_name;
    
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
