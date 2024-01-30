#include <Arduino.h>
#include <ArduinoJson.h>
#include <ConfigStore.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>
#include <WiFi.h>
#include <defaults.h>
#include <utils.h>
/*
Safe area is minimal working system relying on following core modules:
- FS access (required to load config persistant settings)
- Config load (required for network conf/ failsafe mode flag)
- Network access (required for wireless device access)
- Webserver (required for OTA updates)
- OTA updater service

Failsafe or recovery mode will be enabled after any previous failed attempt to
boot by checking a flag indicating if previous boot was successful or not. The
flag value is read and reset immediately at the beginning of boot process. If
booting succeeds flag will be set back to normal at the end of boot process If
not it will remain unset, indicating system boot failure.
*/
class System {
public:
  static void coreInit();
  static void initFs();
  static void loadNetworkProfile(std::string profileName);
  static void wifiConf();
  static void activateWifiAP();
  static void activateWifiSTA();
};

void System::coreInit() {
  LogStore::info("[System::init] Filesystem");
  initFs();
  LogStore::info("[System::init] Network");
  wifiConf();
  activateWifiAP();
  activateWifiSTA();
  // loadNetworkProfile("tplink");
}

void System::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  FsUtils::listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}

void System::loadNetworkProfile(std::string profileName) {
  LogStore::info("[System::loadNetworkProfile] loading network profile: " +
                 profileName);
  JsonObject networkProfiles = ConfigStore::configContent()["networkProfiles"];
  JsonObject profileContent = networkProfiles[profileName];
  std::string profileContentDump = "";
  serializeJsonPretty(profileContent, profileContentDump);
  LogStore::dbg("[System::loadNetworkProfile] dump profile content " +
                profileContentDump);
}

void System::wifiConf() {
  LogStore::info("[System::wifiConf] setting static IP address");
  // TODO load param from config file instead of using hardcoding
  std::string sStaticIP = ConfigStore::configContent()["staticIP"];
  std::string sGateway = ConfigStore::configContent()["gateway"];
  std::string sSubnet = ConfigStore::configContent()["subnet"];
  std::string sPrimaryDNS = ConfigStore::configContent()["primaryDNS"];
  std::string sSecondaryDNS = ConfigStore::configContent()["secondaryDNS"];
  LogStore::info("[System::wifiConf] LAN settings staticIP: " + sStaticIP +
                 ", gateway: " + sGateway + ", subnet: " + sSubnet);
  std::vector<int> viStaticIP =
      strVectToIntVect(splitFromCharDelim(sStaticIP, '.'));
  std::vector<int> viGateway =
      strVectToIntVect(splitFromCharDelim(sGateway, '.'));
  std::vector<int> viSubnet =
      strVectToIntVect(splitFromCharDelim(sSubnet, '.'));
  std::vector<int> viPrimeDns =
      strVectToIntVect(splitFromCharDelim(sPrimaryDNS, '.'));
  std::vector<int> viSecondDNS =
      strVectToIntVect(splitFromCharDelim(sSecondaryDNS, '.'));

  IPAddress local_IP(viStaticIP.at(0), viStaticIP.at(1), viStaticIP.at(2),
                     viStaticIP.at(3));
  IPAddress gateway(viGateway.at(0), viGateway.at(1), viGateway.at(2),
                    viGateway.at(3));
  IPAddress subnet(viSubnet.at(0), viSubnet.at(1), viSubnet.at(2),
                   viSubnet.at(3));
  IPAddress primaryDNS(viPrimeDns.at(0), viPrimeDns.at(1), viPrimeDns.at(2),
                       viPrimeDns.at(3)); // optional
  IPAddress secondaryDNS(viSecondDNS.at(0), viSecondDNS.at(1),
                         viSecondDNS.at(2),
                         viSecondDNS.at(3)); // optional
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    LogStore::info("[System::wifiConf] STA Failed to configure");
  }
}

// turn on wifi access point
void System::activateWifiAP() {
  std::string hotspotSSID = ConfigStore::configContent()["hotspotSSID"];
  std::string hotspotPWD = ConfigStore::configContent()["hotspotPWD"];
  LogStore::info("[Core::WifiAP] Setting up access point " + hotspotSSID);
  WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
  LogStore::info("[Core::WifiAP] Hotspot " + std::string(hotspotSSID.c_str()) +
                 " available at " +
                 std::string(WiFi.softAPIP().toString().c_str()));
}

// turn on wifi client
void System::activateWifiSTA() {
  int retryCounter = 10;
  std::string wifiSSID = ConfigStore::configContent()["wifiSSID"];
  std::string wifiPWD = ConfigStore::configContent()["wifiPWD"];
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