#include <Arduino.h>
#include <ArduinoJson.h>
#include <FileHandlers.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>
#include <WiFi.h>
#include <defaults.h>

class System {
public:
  static const JsonFileHandler *config; 

public:
  static void coreInit();
  static void initFs();
  static void setupWifiAP();
  static void setupWifiSTA();
};

// const JsonFileHandler &System::config = JsonFileHandler(CONFIG_FILE);
const JsonFileHandler *System::config = NULL;

void System::coreInit() {
  LogStore::info("[System::coreInit] Filesystem");
  System::initFs();
  LogStore::info("[System::coreInit] Config");
  System::config = new JsonFileHandler(CONFIG_FILE);
  // static JsonFileHandler config = JsonFileHandler(CONFIG_FILE);
  LogStore::info("[System::coreInit] Network");
  setupWifiAP();
  setupWifiSTA();
}

void System::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  FsUtils::listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}

void System::setupWifiAP() {
  std::string hotspotSSID = System::config->jsFileContent["hotspotSSID"];
  std::string hotspotPWD = System::config->jsFileContent["hotspotPWD"];
  LogStore::info("[Core::WifiAP] Setting up access point " + hotspotSSID);
  WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
  LogStore::info("[Core::WifiAP] Hotspot " + std::string(hotspotSSID.c_str()) +
                 " available at " +
                 std::string(WiFi.softAPIP().toString().c_str()));
}

void System::setupWifiSTA() {
  int retryCounter = 10;
  std::string wifiSSID = System::config->jsFileContent["wifiSSID"];
  std::string wifiPWD = System::config->jsFileContent["wifiPWD"];
  LogStore::info("[Core::WifiSTA] Connecting to " +
                 std::string(wifiSSID.c_str()));
  WiFi.begin(wifiSSID.c_str(), wifiPWD.c_str());

  // wait for connection
  while (retryCounter > 0 && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCounter--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    std::string ipAddr(WiFi.localIP().toString().c_str());
    LogStore::info("[Core::WifiSTA] Connected with address: " + ipAddr);
  } else {
    LogStore::info("[Core::WifiSTA] Could not connect to LAN");
  }
}