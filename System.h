#include <Arduino.h>
#include <ArduinoJson.h>
#include <ConfigStore.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>
#include <WiFi.h>
#include <defaults.h>

class System {
public:
  static void coreInit();
  static void initFs();
  static void wifiConf();
  static void activateWifiAP();
  static void activateWifiSTA();
};

void System::coreInit() {
  LogStore::info("[System::init] Filesystem");
  initFs();
  LogStore::info("[System::init] Config");
  ConfigStore::init();
  LogStore::info("[System::init] Network");
  wifiConf();
  activateWifiAP();
  activateWifiSTA();
}

void System::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  FsUtils::listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}

void System::wifiConf() {
  LogStore::info("[System::wifiConf] setting static IP address");
  // TODO load param from config file instead of using hardcoding
  IPAddress local_IP(192, 168, 1, 201);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   // optional
  IPAddress secondaryDNS(8, 8, 4, 4); // optional
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    LogStore::info("[System::wifiConf] STA Failed to configure");
  }
}

// turn on wifi access point
void System::activateWifiAP() {
  std::string hotspotSSID = ConfigStore::setting("hotspotSSID");
  std::string hotspotPWD = ConfigStore::setting("hotspotPWD");
  LogStore::info("[Core::WifiAP] Setting up access point " + hotspotSSID);
  WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
  LogStore::info("[Core::WifiAP] Hotspot " + std::string(hotspotSSID.c_str()) +
                 " available at " +
                 std::string(WiFi.softAPIP().toString().c_str()));
}

// turn on wifi client
void System::activateWifiSTA() {
  int retryCounter = 10;
  std::string wifiSSID = ConfigStore::setting("wifiSSID");
  std::string wifiPWD = ConfigStore::setting("wifiPWD");
  LogStore::info("[Core::WifiSTA] Connecting to " +
                 std::string(wifiSSID.c_str()));
  // any wifi conf must be called prior to this
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