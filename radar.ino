/*
 * ============================================================
 *  ADVANCED 360° DUAL-ARRAY RADAR — ESP8266 FIRMWARE
 * ============================================================
 *
 *  DUAL ACCESS MODES
 *  ─────────────────
 *  1. AP MODE  (always on)
 *     Connect to the ESP's own Wi-Fi hotspot:
 *       SSID     : RadarNode-001   (changeable in /config)
 *       Password : radar@secure99  (changeable in /config)
 *     Then open:
 *       http://192.168.4.1/        → radar dashboard
 *       http://192.168.4.1/config  → config page
 *
 *  2. STA MODE (after Wi-Fi credentials saved in /config)
 *     ESP connects to your router. Access same URLs over LAN.
 *     IP printed on Serial monitor after connect.
 *
 *  CAPTIVE PORTAL (automatic)
 *  ───────────────────────────
 *  If the saved Wi-Fi is unreachable for 60 seconds (at boot
 *  or mid-operation), the ESP enters captive-portal mode:
 *    • A DNS server answers ALL hostnames with the AP IP.
 *    • Any browser connected to the AP is redirected to /config
 *      so you can enter new Wi-Fi credentials immediately.
 *    • Radar sweep keeps running throughout.
 *    • Captive mode exits automatically once STA reconnects.
 *
 *  CONFIG PAGE  http://<ip>/config
 *  ─────────────────────────────────
 *  Protected by CONFIG_USER / CONFIG_PASS (hardcoded — never
 *  stored in EEPROM, never changeable via UI).
 *  Lets you set:
 *    • Target Wi-Fi SSID + WPA2 password
 *    • Radar dashboard username + password
 *    • ESP AP SSID + password
 *  All settings persisted to EEPROM (512 bytes).
 *
 *  RADAR DASHBOARD  http://<ip>/
 *  ─────────────────────────────
 *  Protected by dashboard credentials (EEPROM).
 *  Defaults: admin / radar1234
 *
 * ============================================================
 */


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <Servo.h>
#include <EEPROM.h>
#include "radar_index_html.h"

// ─── MASTER HARDCODED CONFIG CREDENTIALS ───────────────
#define CONFIG_USER   "superadmin"
#define CONFIG_PASS   "cfg@Master77"

// ─── EEPROM ADDRESS LAYOUT ─────────────────────────────────
#define EEPROM_SIZE        512
#define ADDR_MAGIC           0   
#define ADDR_WIFI_SSID       2   
#define ADDR_WIFI_PASS      34   
#define ADDR_DASH_USER      98   
#define ADDR_DASH_PASS     130   
#define ADDR_AP_SSID       162   
#define ADDR_AP_PASS       194   

// ─── RUNTIME SYSTEM SETTINGS ───────────────────────────────
struct Config {
    char wifiSSID[33]   = "";
    char wifiPass[65]   = "";
    char dashUser[33]   = "admin";
    char dashPass[33]   = "radar1234";
    char apSSID[33]     = "RadarNode-001";
    char apPass[33]     = "radar@secure99";
} cfg;

// ─── HARDWARE PINS ─────────────────────────────────────────
#define SERVO_PIN    D4
#define TRIG_PIN_1   D1
#define ECHO_PIN_1   D2
#define TRIG_PIN_2   D5
#define ECHO_PIN_2   D6

Servo radarServo;
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

unsigned long lastSweepUpdate = 0;
int  currentServoAngle = 0;
int  sweepIncrement    = 1;

DNSServer        dnsServer;
bool             captiveMode        = false;
unsigned long    staLostSince       = 0;
bool             staWasConnected    = false;
#define STA_TIMEOUT_MS   60000UL                
#define DNS_PORT         53

void eepromWriteStr(int addr, const char* str, int maxLen) {
    for (int i = 0; i < maxLen; i++) {
        EEPROM.write(addr + i, (i < (int)strlen(str)) ? str[i] : 0);
    }
}

void eepromReadStr(int addr, char* buf, int maxLen) {
    for (int i = 0; i < maxLen; i++) {
        buf[i] = EEPROM.read(addr + i);
    }
    buf[maxLen - 1] = '\0';
}

bool eepromHasValidConfig() {
    return (EEPROM.read(ADDR_MAGIC) == 0xAB && EEPROM.read(ADDR_MAGIC + 1) == 0xCD);
}

void saveConfig() {
    EEPROM.write(ADDR_MAGIC,     0xAB);
    EEPROM.write(ADDR_MAGIC + 1, 0xCD);
    eepromWriteStr(ADDR_WIFI_SSID,  cfg.wifiSSID,  33);
    eepromWriteStr(ADDR_WIFI_PASS,  cfg.wifiPass,  65);
    eepromWriteStr(ADDR_DASH_USER,  cfg.dashUser,  33);
    eepromWriteStr(ADDR_DASH_PASS,  cfg.dashPass,  33);
    eepromWriteStr(ADDR_AP_SSID,    cfg.apSSID,    33);
    eepromWriteStr(ADDR_AP_PASS,    cfg.apPass,    33);
    EEPROM.commit();
}

void loadConfig() {
    if (!eepromHasValidConfig()) return;
    eepromReadStr(ADDR_WIFI_SSID,  cfg.wifiSSID,  33);
    eepromReadStr(ADDR_WIFI_PASS,  cfg.wifiPass,  65);
    eepromReadStr(ADDR_DASH_USER,  cfg.dashUser,  33);
    eepromReadStr(ADDR_DASH_PASS,  cfg.dashPass,  33);
    eepromReadStr(ADDR_AP_SSID,    cfg.apSSID,    33);
    eepromReadStr(ADDR_AP_PASS,    cfg.apPass,    33);
}

void enterCaptiveMode() {
    if (captiveMode) return;
    captiveMode = true;
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

void exitCaptiveMode() {
    if (!captiveMode) return;
    captiveMode = false;
    dnsServer.stop();
}

bool redirectIfCaptive() {
    if (!captiveMode) return false;
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/config", true);
    server.send(302, "text/plain", "");
    return true;
}

long readDistanceCM(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH, 30000);
    if (duration == 0) return -1;
    return duration * 0.0343 / 2;
}

// ============================================================
//  WEB SERVER ROUTES & AUTHORIZATIONS
// ============================================================

bool checkConfigAuth() {
    if (!server.authenticate(CONFIG_USER, CONFIG_PASS)) {
        server.requestAuthentication(DIGEST_AUTH, "Config Panel", "Authentication required to access configuration.");
        return false;
    }
    return true;
}

bool checkDashAuth() {
    if (!server.authenticate(cfg.dashUser, cfg.dashPass)) {
        server.requestAuthentication(DIGEST_AUTH, "Radar Dashboard", "Authentication required.");
        return false;
    }
    return true;
}

void serveConfigPage() {
    if (!checkConfigAuth()) return;
    server.send_P(200, "text/html", config_html);
}

void serveDashboard() {
    if (redirectIfCaptive()) return;
    if (!checkDashAuth()) return;
    server.send_P(200, "text/html", index_html);
}

void handleWifiScan() {
    if (!checkConfigAuth()) return;
    int n = WiFi.scanNetworks(false, true); // Blocking scan, show hidden
    String json = "[";
    for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        json += "{\"ssid\":\"";
        String ssid = WiFi.SSID(i);
        ssid.replace("\"", "\\\"");
        json += ssid + "\",\"rssi\":" + String(WiFi.RSSI(i)) + ",\"enc\":";
        json += (WiFi.encryptionType(i) != ENC_TYPE_NONE) ? "true" : "false";
        json += "}";
    }
    json += "]";
    WiFi.scanDelete();
    server.send(200, "application/json", json);
}

void handleWifiConnect() {
    if (!checkConfigAuth()) return;
    if (!server.hasArg("ssid") || server.arg("ssid").length() == 0) {
        server.send(400, "application/json", "{\"connected\":false,\"error\":\"No SSID supplied\"}");
        return;
    }
    String newSSID = server.arg("ssid");
    String newPass = server.hasArg("pass") ? server.arg("pass") : "";
    newSSID.toCharArray(cfg.wifiSSID, 33);
    newPass.toCharArray(cfg.wifiPass, 65);
    saveConfig();

    WiFi.begin(cfg.wifiSSID, cfg.wifiPass);
    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 15000) {
        delay(300);
        server.handleClient();
    }

    if (WiFi.status() == WL_CONNECTED) {
        staWasConnected = true;
        staLostSince    = 0;
        exitCaptiveMode();
        server.send(200, "application/json", "{\"connected\":true,\"ip\":\"" + WiFi.localIP().toString() + "\"}");
    } else {
        server.send(200, "application/json", "{\"connected\":false,\"error\":\"Failed to establish link.\"}");
    }
}

void handleWifiStatus() {
    if (!checkConfigAuth()) return;
    String json = "{\"connected\":";
    if (WiFi.status() == WL_CONNECTED) {
        String ssid = WiFi.SSID();
        ssid.replace("\"", "\\\"");
        json += "true,\"ssid\":\"" + ssid + "\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    } else {
        json += "false,\"ssid\":\"\",\"ip\":\"\"}";
    }
    server.send(200, "application/json", json);
}

void handleConfigSave() {
    if (!checkConfigAuth()) return;
    if (server.hasArg("apssid") && server.arg("apssid").length() >= 1) server.arg("apssid").toCharArray(cfg.apSSID, 33);
    if (server.hasArg("appass") && server.arg("appass").length() >= 8) server.arg("appass").toCharArray(cfg.apPass, 33);
    if (server.hasArg("duser") && server.arg("duser").length() > 0) server.arg("duser").toCharArray(cfg.dashUser, 33);
    if (server.hasArg("dpass") && server.arg("dpass").length() > 0) server.arg("dpass").toCharArray(cfg.dashPass, 33);
    saveConfig();
    server.send(200, "application/json", F("{\"ok\":true}"));
}

void handleReboot() {
    if (!checkConfigAuth()) return;
    server.send(200, "text/plain", "Rebooting...");
    delay(500);
    ESP.restart();
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {}

// ============================================================
//  MAIN EXECUTION
// ============================================================

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    loadConfig();

    pinMode(TRIG_PIN_1, OUTPUT); pinMode(ECHO_PIN_1, INPUT);
    pinMode(TRIG_PIN_2, OUTPUT); pinMode(ECHO_PIN_2, INPUT);
    radarServo.attach(SERVO_PIN);
    radarServo.write(0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(cfg.apSSID, cfg.apPass);

    if (strlen(cfg.wifiSSID) > 0) {
        WiFi.begin(cfg.wifiSSID, cfg.wifiPass);
        unsigned long t = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - t < STA_TIMEOUT_MS) {
            delay(500);
            server.handleClient();
        }
        if (WiFi.status() != WL_CONNECTED) enterCaptiveMode();
    } else {
        enterCaptiveMode();
    }

    server.on("/",             HTTP_GET,  serveDashboard);
    server.on("/config",       HTTP_GET,  serveConfigPage);
    server.on("/config/save",  HTTP_POST, handleConfigSave);
    server.on("/reboot",       HTTP_POST, handleReboot);
    server.on("/wifi/scan",    HTTP_GET,  handleWifiScan);
    server.on("/wifi/connect", HTTP_POST, handleWifiConnect);
    server.on("/wifi/status",  HTTP_GET,  handleWifiStatus);
    server.begin();
    
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void loop() {
    if (captiveMode) dnsServer.processNextRequest();
    server.handleClient();
    webSocket.loop();

    if (strlen(cfg.wifiSSID) > 0) {
        if (WiFi.status() == WL_CONNECTED) {
            if (captiveMode) exitCaptiveMode();
            staWasConnected = true;
            staLostSince    = 0;
        } else {
            if (staLostSince == 0) {
                staLostSince = millis();
                WiFi.begin(cfg.wifiSSID, cfg.wifiPass);
            } else if (!captiveMode && (millis() - staLostSince >= STA_TIMEOUT_MS)) {
                enterCaptiveMode();
            }
        }
    }

    if (millis() - lastSweepUpdate >= 40) {
        lastSweepUpdate = millis();
        radarServo.write(currentServoAngle);

        long dist1 = readDistanceCM(TRIG_PIN_1, ECHO_PIN_1);
        long dist2 = readDistanceCM(TRIG_PIN_2, ECHO_PIN_2);

        int angleS1 = currentServoAngle;          
        int angleS2 = currentServoAngle + 180;    

        String payload = String(angleS1) + "," + String(dist1) + "|" + String(angleS2) + "," + String(dist2);
        webSocket.broadcastTXT(payload);

        currentServoAngle += sweepIncrement;
        if (currentServoAngle >= 179 || currentServoAngle <= 0) {
            sweepIncrement *= -1;
        }
    }
}