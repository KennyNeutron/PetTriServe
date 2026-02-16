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
        .form-group input { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
        .form-group input.time-part { width: 48%; text-align: center; }
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
        
        <hr style="margin: 2rem 0; border: 0; border-top: 1px solid #d2d2d7;">
        <h3>Feeder Settings</h3>
        
        <div class="info-card">
            <strong>Feeder 1</strong>
            <div class="form-group"><label>Start Time</label><input type="time" id="f1_start"></div>
            <div class="form-group"><label>Interval (Hrs : Min)</label>
                <div style="display:flex; align-items:center; gap:5px;">
                    <input type="number" id="f1_int_h" placeholder="HH" min="0" max="23" class="time-part">
                    <span>:</span>
                    <input type="number" id="f1_int_m" placeholder="MM" min="0" max="59" class="time-part">
                </div>
            </div>
            <div class="form-group"><label>Duration (Seconds)</label><input type="number" id="f1_dur" min="1" max="60" value="5"></div>
        </div>
        
        <div class="info-card">
            <strong>Feeder 2</strong>
            <div class="form-group"><label>Start Time</label><input type="time" id="f2_start"></div>
            <div class="form-group"><label>Interval (Hrs : Min)</label>
                <div style="display:flex; align-items:center; gap:5px;">
                    <input type="number" id="f2_int_h" placeholder="HH" min="0" max="23" class="time-part">
                    <span>:</span>
                    <input type="number" id="f2_int_m" placeholder="MM" min="0" max="59" class="time-part">
                </div>
            </div>
            <div class="form-group"><label>Duration (Seconds)</label><input type="number" id="f2_dur" min="1" max="60" value="5"></div>
        </div>

        <div class="info-card">
            <strong>Feeder 3</strong>
            <div class="form-group"><label>Start Time</label><input type="time" id="f3_start"></div>
            <div class="form-group"><label>Interval (Hrs : Min)</label>
                <div style="display:flex; align-items:center; gap:5px;">
                    <input type="number" id="f3_int_h" placeholder="HH" min="0" max="23" class="time-part">
                    <span>:</span>
                    <input type="number" id="f3_int_m" placeholder="MM" min="0" max="59" class="time-part">
                </div>
            </div>
            <div class="form-group"><label>Duration (Seconds)</label><input type="number" id="f3_dur" min="1" max="60" value="5"></div>
        </div>
        
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
            
            const f1_start = document.getElementById('f1_start').value;
            const f1_int_h = document.getElementById('f1_int_h').value.padStart(2, '0');
            const f1_int_m = document.getElementById('f1_int_m').value.padStart(2, '0');
            const f1_int = `${f1_int_h}:${f1_int_m}`;
            const f1_dur = document.getElementById('f1_dur').value;
            
            const f2_start = document.getElementById('f2_start').value;
            const f2_int_h = document.getElementById('f2_int_h').value.padStart(2, '0');
            const f2_int_m = document.getElementById('f2_int_m').value.padStart(2, '0');
            const f2_int = `${f2_int_h}:${f2_int_m}`;
            const f2_dur = document.getElementById('f2_dur').value;
            
            const f3_start = document.getElementById('f3_start').value;
            const f3_int_h = document.getElementById('f3_int_h').value.padStart(2, '0');
            const f3_int_m = document.getElementById('f3_int_m').value.padStart(2, '0');
            const f3_int = `${f3_int_h}:${f3_int_m}`;
            const f3_dur = document.getElementById('f3_dur').value;

            const status = document.getElementById('status');
            const btn = document.getElementById('saveBtn');
            if (!ssid) { alert("Please select a WiFi network"); return; }
            status.style.display = 'block'; status.textContent = "Saving..."; status.className = "status-msg"; btn.disabled = true;
            try {
                const formData = new URLSearchParams();
                formData.append('ssid', ssid); formData.append('password', pass); formData.append('device_name', name);
                
                formData.append('f1_start', f1_start); formData.append('f1_int', f1_int); formData.append('f1_dur', f1_dur);
                formData.append('f2_start', f2_start); formData.append('f2_int', f2_int); formData.append('f2_dur', f2_dur);
                formData.append('f3_start', f3_start); formData.append('f3_int', f3_int); formData.append('f3_dur', f3_dur);
                
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
                
                if (data.f1_start) document.getElementById('f1_start').value = data.f1_start;
                if (data.f1_int) {
                    const parts = data.f1_int.split(':');
                    if(parts.length >= 2) {
                        document.getElementById('f1_int_h').value = parseInt(parts[0]);
                        document.getElementById('f1_int_m').value = parseInt(parts[1]);
                    }
                }
                if (data.f1_dur) document.getElementById('f1_dur').value = data.f1_dur;
                
                if (data.f2_start) document.getElementById('f2_start').value = data.f2_start;
                if (data.f2_int) {
                    const parts = data.f2_int.split(':');
                    if(parts.length >= 2) {
                        document.getElementById('f2_int_h').value = parseInt(parts[0]);
                        document.getElementById('f2_int_m').value = parseInt(parts[1]);
                    }
                }
                if (data.f2_dur) document.getElementById('f2_dur').value = data.f2_dur;
                
                if (data.f3_start) document.getElementById('f3_start').value = data.f3_start;
                if (data.f3_int) {
                    const parts = data.f3_int.split(':');
                    if(parts.length >= 2) {
                        document.getElementById('f3_int_h').value = parseInt(parts[0]);
                        document.getElementById('f3_int_m').value = parseInt(parts[1]);
                    }
                }
                if (data.f3_dur) document.getElementById('f3_dur').value = data.f3_dur;
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

FeederConfig Portal32::getFeeder(int id) {
    if (id < 1 || id > 3) return {0, "", ""};
    return feeders[id-1];
}

void Portal32::loadSettings() {
    preferences.begin("portal32", true);
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    device_name = preferences.getString("device_name", "Portal32-Device");
    
    feeders[0].id = 1;
    feeders[0].startTime = preferences.getString("f1_start", "08:00");
    feeders[0].interval = preferences.getString("f1_int", "04:00");
    feeders[0].dispenseDuration = preferences.getInt("f1_dur", 5);

    feeders[1].id = 2;
    feeders[1].startTime = preferences.getString("f2_start", "12:00");
    feeders[1].interval = preferences.getString("f2_int", "04:00");
    feeders[1].dispenseDuration = preferences.getInt("f2_dur", 5);

    feeders[2].id = 3;
    feeders[2].startTime = preferences.getString("f3_start", "18:00");
    feeders[2].interval = preferences.getString("f3_int", "04:00");
    feeders[2].dispenseDuration = preferences.getInt("f3_dur", 5);
    
    preferences.end();

    Serial.printf("[PORTAL32] Loaded Settings:\n");
    Serial.printf("  SSID: %s\n", ssid.c_str());
    Serial.printf("  F1 Start: %s, Int: %s, Dur: %d\n", feeders[0].startTime.c_str(), feeders[0].interval.c_str(), feeders[0].dispenseDuration);
    Serial.printf("  F2 Start: %s, Int: %s, Dur: %d\n", feeders[1].startTime.c_str(), feeders[1].interval.c_str(), feeders[1].dispenseDuration);
    Serial.printf("  F3 Start: %s, Int: %s, Dur: %d\n", feeders[2].startTime.c_str(), feeders[2].interval.c_str(), feeders[2].dispenseDuration);}

void Portal32::saveSettings(String s, String p, String n) {
    // This overload is kept for compatibility but main saving logic is in handleSave for specific fields
    // or we can update this to take all params, but handleSave does it directly to prefs usually
    // Let's just use it for basic wifi if needed, but better to do it in handleSave or separate method.
    // For now, I will update this to just save what's passed, but handleSave will call preferences directly.
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
    
    // Feeder settings
    String f1_s = server.arg("f1_start");
    String f1_i = server.arg("f1_int");
    String f2_s = server.arg("f2_start");
    String f2_i = server.arg("f2_int");
    String f3_s = server.arg("f3_start");
    String f3_i = server.arg("f3_int");
    
    // Debugging
    Serial.printf("[PORTAL32] Saving Config:\n");
    Serial.printf("  SSID: %s\n", s.c_str());
    Serial.printf("  Name: %s\n", n.c_str());
    Serial.printf("  F1 Start: %s, Int: %s, Dur: %d\n", f1_s.c_str(), f1_i.c_str(), server.arg("f1_dur").toInt());
    Serial.printf("  F2 Start: %s, Int: %s, Dur: %d\n", f2_s.c_str(), f2_i.c_str(), server.arg("f2_dur").toInt());
    Serial.printf("  F3 Start: %s, Int: %s, Dur: %d\n", f3_s.c_str(), f3_i.c_str(), server.arg("f3_dur").toInt());

    if (n.length() == 0) n = "Portal32-Device";
    Serial.println("[PORTAL32] New configuration received. Saving...");
    
    preferences.begin("portal32", false);
    
    // Clear potentially corrupted keys by overwriting
    preferences.putString("ssid", s);
    preferences.putString("password", p);
    preferences.putString("device_name", n);
    
    preferences.putString("f1_start", f1_s);
    preferences.putString("f1_int", f1_i);
    preferences.putInt("f1_dur", server.arg("f1_dur").toInt());
    
    preferences.putString("f2_start", f2_s);
    preferences.putString("f2_int", f2_i);
    preferences.putInt("f2_dur", server.arg("f2_dur").toInt());
    
    preferences.putString("f3_start", f3_s);
    preferences.putString("f3_int", f3_i);
    preferences.putInt("f3_dur", server.arg("f3_dur").toInt());
    preferences.end();

    // Reload settings to update memory
    loadSettings();

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
    json += "\"device\":\"" + device_name + "\",";
    
    json += "\"f1_start\":\"" + feeders[0].startTime + "\",";
    json += "\"f1_int\":\"" + feeders[0].interval + "\",";
    json += "\"f2_start\":\"" + feeders[1].startTime + "\",";
    json += "\"f2_int\":\"" + feeders[1].interval + "\",";
    json += "\"f3_start\":\"" + feeders[2].startTime + "\",";
    json += "\"f3_int\":\"" + feeders[2].interval + "\"";
    
    json += "}";
    server.send(200, "application/json", json);
}
