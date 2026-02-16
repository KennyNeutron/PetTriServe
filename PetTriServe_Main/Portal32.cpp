#include "Portal32.h"

Portal32 portal;

const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Portal32 Setup</title>
    <style>
        :root { --primary: #007aff; --bg: #f5f5f7; --card: #ffffff; --text: #1d1d1f; --secondary: #86868b; }
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background-color: var(--bg); color: var(--text); margin: 0; display: flex; justify-content: center; align-items: center; min-height: 100vh; }
        .container { width: 90%; max-width: 400px; background: var(--card); padding: 2rem; border-radius: 18px; box-shadow: 0 10px 30px rgba(0,0,0,0.05); }
        h1 { font-size: 24px; font-weight: 600; margin-bottom: 1.5rem; text-align: center; }
        .form-group { margin-bottom: 1.2rem; }
        label { display: block; font-size: 14px; color: var(--secondary); margin-bottom: 0.4rem; }
        input, select { width: 100%; padding: 12px; border: 1px solid #d2d2d7; border-radius: 8px; font-size: 16px; box-sizing: border-box; outline: none; transition: border-color 0.2s; }
        input:focus { border-color: var(--primary); }
        .btn { width: 100%; padding: 14px; background: var(--primary); color: white; border: none; border-radius: 10px; font-size: 16px; font-weight: 500; cursor: pointer; margin-top: 1rem; }
        .btn:disabled { background: #b1b1b1; }
        .status-msg { margin-top: 1rem; font-size: 14px; text-align: center; padding: 10px; border-radius: 8px; display: none; }
        .success { background: #e8f5e9; color: #2e7d32; }
        .error { background: #ffebee; color: #c62828; }
        .scan-btn { font-size: 12px; color: var(--primary); cursor: pointer; float: right; }
        .info-card { background: #f8f9fa; padding: 12px; border-radius: 10px; margin-bottom: 1.5rem; font-size: 14px; border: 1px solid #e9ecef; }
        .info-row { display: flex; justify-content: space-between; margin-bottom: 5px; }
        .info-label { color: var(--secondary); font-weight: 500; }
        .badge { padding: 2px 8px; border-radius: 12px; font-size: 11px; font-weight: 700; text-transform: uppercase; }
        .badge-green { background: #e8f5e9; color: #2e7d32; }
        .badge-gray { background: #f1f3f4; color: #5f6368; }
        .badge-red { background: #ffebee; color: #c62828; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Portal32</h1>
        <div id="connection-info" class="info-card">
            <div class="info-row"><span class="info-label">Network:</span><span id="curr-ssid">--</span></div>
            <div class="info-row"><span class="info-label">Connection:</span><span id="conn-status" class="badge">Checking...</span></div>
            <div class="info-row"><span class="info-label">Internet:</span><span id="net-status" class="badge">Checking...</span></div>
        </div>
        <div class="form-group">
            <span class="scan-btn" onclick="scanWifi()">Refresh Scan</span>
            <label for="ssid">WiFi Network (SSID)</label>
            <select id="ssid_select" onchange="document.getElementById('ssid').value = this.value"><option value="">Scanning...</option></select>
            <input type="hidden" id="ssid">
        </div>
        <div class="form-group"><label for="pass">Password</label><input type="password" id="pass" placeholder="Enter WiFi Password"></div>
        <div class="form-group"><label for="name">Device Name</label><input type="text" id="name" placeholder="Portal32-Device"></div>
        <button class="btn" id="saveBtn" onclick="saveConfig()">Save & Connect</button>
        <div id="status" class="status-msg"></div>
    </div>
    <script>
        async function scanWifi() {
            const select = document.getElementById('ssid_select');
            select.innerHTML = '<option>Scanning...</option>';
            try {
                const res = await fetch('/scan');
                const networks = await res.json();
                select.innerHTML = '<option value="">Select a network</option>';
                networks.forEach(net => {
                    const opt = document.createElement('option');
                    opt.value = net.ssid;
                    opt.textContent = `${net.ssid} (${net.rssi}dBm)`;
                    select.appendChild(opt);
                });
            } catch (e) { console.error("Scan failed", e); }
        }
        async function saveConfig() {
            const ssid = document.getElementById('ssid').value;
            const pass = document.getElementById('pass').value;
            const name = document.getElementById('name').value;
            const status = document.getElementById('status');
            const btn = document.getElementById('saveBtn');
            if (!ssid) { alert("Please select a WiFi network"); return; }
            status.style.display = 'block'; status.textContent = "Saving..."; status.className = "status-msg"; btn.disabled = true;
            try {
                const formData = new URLSearchParams();
                formData.append('ssid', ssid); formData.append('password', pass); formData.append('device_name', name);
                const res = await fetch('/save', { method: 'POST', body: formData });
                if (res.ok) { status.textContent = "Success! Rebooting ESP32..."; status.className = "status-msg success"; } 
                else { throw new Error("Save failed"); }
            } catch (e) { status.textContent = "Error saving settings."; status.className = "status-msg error"; btn.disabled = false; }
        }
        async function checkStatus() {
            try {
                const res = await fetch('/status');
                const data = await res.json();
                document.getElementById('curr-ssid').textContent = data.current_ssid || "Not Connected";
                const conn = document.getElementById('conn-status');
                conn.textContent = data.status; conn.className = 'badge ' + (data.status === 'connected' ? 'badge-green' : 'badge-gray');
                const net = document.getElementById('net-status');
                net.textContent = data.internet ? 'Access' : 'No Access'; net.className = 'badge ' + (data.internet ? 'badge-green' : 'badge-red');
                if (data.device) document.getElementById('name').value = data.device;
            } catch (e) { console.error("Status check failed", e); }
        }
        window.onload = () => { scanWifi(); checkStatus(); setInterval(checkStatus, 5000); };
    </script>
</body>
</html>
)=====";

Portal32::Portal32() : server(80), apIP(192, 168, 4, 1) {
    isAPMode = false;
    isConnecting = false;
    shouldConnect = false;
    connectionStartTime = 0;
    lastStatusCheck = 0;
}

void Portal32::begin() {
    Serial.println("\n\n[PORTAL32] Booting...");
    loadSettings();

    if (ssid.length() > 0) {
        WiFi.mode(WIFI_STA);
    } else {
        WiFi.mode(WIFI_AP_STA);
    }

    initWebServer();

    if (ssid.length() > 0) {
        startSTA();
    } else {
        Serial.println("[PORTAL32] No credentials found. Starting AP...");
        setupAP();
    }
}

void Portal32::handle() {
    if (isAPMode) {
        dnsServer.processNextRequest();
    }
    server.handleClient();

    if (shouldConnect) {
        shouldConnect = false;
        startSTA();
    }

    if (isConnecting) {
        if (WiFi.status() == WL_CONNECTED) {
            isConnecting = false;
            isAPMode = false;
            Serial.println("\n[PORTAL32] Connected!");
            Serial.print("[PORTAL32] IP Address: ");
            Serial.println(WiFi.localIP());
            
            if (MDNS.begin(device_name.c_str())) {
                Serial.println("[MDNS] Started: " + device_name + ".local");
            }
        } else if (millis() - connectionStartTime > 20000) {
            isConnecting = false;
            Serial.println("\n[PORTAL32] Connection failed (timeout).");
            if (!isAPMode) setupAP();
        }
    }

    if (!isAPMode && !isConnecting && WiFi.status() != WL_CONNECTED && millis() - lastStatusCheck > 30000) {
        Serial.println("[PORTAL32] Connection lost. Re-attempting...");
        startSTA();
        lastStatusCheck = millis();
    }
}

bool Portal32::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool Portal32::isWifiConnecting() { return isConnecting; }
bool Portal32::isAPModeActive() { return isAPMode; }

String Portal32::getSSID() { return ssid; }
String Portal32::getIP() { return WiFi.localIP().toString(); }
String Portal32::getDeviceName() { return device_name; }

void Portal32::loadSettings() {
    preferences.begin("portal32", true);
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    device_name = preferences.getString("device_name", "Portal32-Device");
    preferences.end();
}

void Portal32::saveSettings(String s, String p, String n) {
    preferences.begin("portal32", false);
    preferences.putString("ssid", s);
    preferences.putString("password", p);
    preferences.putString("device_name", n);
    preferences.end();
}

void Portal32::setupAP() {
    isAPMode = true;
    isConnecting = false;
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID)) {
        Serial.print("[PORTAL32] AP Mode started. SSID: "); Serial.println(AP_SSID);
        Serial.print("[PORTAL32] IP Address: "); Serial.println(WiFi.softAPIP());
        dnsServer.start(DNS_PORT, "*", apIP);
    } else {
        Serial.println("[PORTAL32] Failed to start SoftAP");
    }
}

void Portal32::startSTA() {
    isConnecting = true;
    connectionStartTime = millis();
    if (isAPMode) WiFi.mode(WIFI_AP_STA); else WiFi.mode(WIFI_STA);
    if(device_name.length() > 0) WiFi.setHostname(device_name.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.printf("[PORTAL32] Connecting to %s...\n", ssid.c_str());
}

void Portal32::initWebServer() {
    server.on("/", std::bind(&Portal32::handleRoot, this));
    server.on("/scan", std::bind(&Portal32::handleScan, this));
    server.on("/save", HTTP_POST, std::bind(&Portal32::handleSave, this));
    server.on("/status", std::bind(&Portal32::handleStatus, this));
    server.onNotFound(std::bind(&Portal32::handleRoot, this)); // Captive Portal redirect
    server.begin();
    Serial.println("[PORTAL32] Web Server started");
}

void Portal32::handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void Portal32::handleScan() {
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; ++i) {
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"encrypted\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        json += "}";
        if (i < n - 1) json += ",";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void Portal32::handleSave() {
    if (!server.hasArg("ssid")) { server.send(400, "text/plain", "Missing Parameters"); return; }
    String s = server.arg("ssid");
    String p = server.arg("password");
    String n = server.arg("device_name");
    if (n.length() == 0) n = "Portal32-Device";
    Serial.println("[PORTAL32] New configuration received. Saving...");
    saveSettings(s, p, n);
    ssid = s; password = p; device_name = n;
    server.send(200, "application/json", "{\"status\":\"ok\"}");
    shouldConnect = true;
}

void Portal32::handleStatus() {
    bool internet = (WiFi.status() == WL_CONNECTED && WiFi.localIP()[0] != 0);
    String json = "{";
    json += "\"status\":\"" + String(WiFi.status() == WL_CONNECTED ? "connected" : "idle") + "\",";
    json += "\"current_ssid\":\"" + (WiFi.status() == WL_CONNECTED ? WiFi.SSID() : "") + "\",";
    json += "\"internet\":" + String(internet ? "true" : "false") + ",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"device\":\"" + device_name + "\"";
    json += "}";
    server.send(200, "application/json", json);
}
