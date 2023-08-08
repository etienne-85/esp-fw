#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>
#include <defaults.h>

class CoreModules {
  static StaticJsonDocument<DEFAULT_JSON_SIZE> jsConfig;

public:
  static void initFs();
  static void loadConfigFile();
  static void setupNetwork();
  static void setupWifiAP();
  static void setupWifiSTA();
};

StaticJsonDocument<DEFAULT_JSON_SIZE> CoreModules::jsConfig;

void CoreModules::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}

/**
 *   Load config from external file stored on device FS
 */
void CoreModules::loadConfigFile() {
  LogStore::info("[Core::ConfigLoader] Loading config from filesystem " +
                 std::string(CONFIG_FILE));

  std::string fileContent = readFile(LittleFS, CONFIG_FILE);
  // convert to a json object
  DeserializationError error = deserializeJson(jsConfig, fileContent);

  // Test if parsing succeeds.
  if (!error) {
    LogStore::info("[Core::ConfigLoader] Done parsing configuration");
  } else {
    LogStore::info("deserializeJson() failed: ");
    Serial.println(error.f_str());
  }
}

void CoreModules::setupNetwork() {
  LogStore::info("[Core::Network] init");
  setupWifiAP();
  setupWifiSTA();
}

void CoreModules::setupWifiAP() {
  std::string hotspotSSID = CoreModules::jsConfig["hotspotSSID"];
  std::string hotspotPWD = CoreModules::jsConfig["hotspotPWD"];
  LogStore::info("[Core::WifiAP] Setting up access point " + hotspotSSID);
  WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
  LogStore::info("[Core::WifiAP] Hotspot " + std::string(hotspotSSID.c_str()) +
                 " available at " + std::string(WiFi.softAPIP().toString().c_str()));
}

void CoreModules::setupWifiSTA() {
  int retryCounter = 10;
  std::string wifiSSID = CoreModules::jsConfig["wifiSSID"];
  std::string wifiPWD = CoreModules::jsConfig["wifiPWD"];
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