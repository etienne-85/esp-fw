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
  static void initLan();
  static void loadLanProfile(std::string profileName);
  static void staticIpConfig(std::string sStaticIP, std::string sGateway,
                             std::string sSubnet, std::string sDns1,
                             std::string sDns2);
  static void activateWifiAP(std::string ssid, std::string pwd);
  static void activateWifiSTA(std::string ssid, std::string pwd);
};

void System::coreInit() {
  LogStore::info("[System::init] Filesystem");
  initFs();
  LogStore::info("[System::init] LAN");
  initLan();
  // activateWifiSTA();
}

void System::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  FsUtils::listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}

void System::initLan() {
  std::string apProfile = ConfigStore::configContent()["usedApProfile"];
  std::string staProfile = ConfigStore::configContent()["usedStaProfile"];
  LogStore::info("[System::initLan] using LAN profiles STA: " + staProfile +
                 ", AP: " + apProfile);
  std::string apBaseSSID =
      ConfigStore::configContent()["networkProfiles"][apProfile]["ssid"];
  std::string apPWD =
      ConfigStore::configContent()["networkProfiles"][apProfile]["pwd"];
  std::string deviceId = ConfigStore::configContent()["deviceId"];
  std::string apFullSSID = apBaseSSID + "-" + deviceId;
  activateWifiAP(apFullSSID, apPWD);
  loadLanProfile(staProfile);
}

void System::loadLanProfile(std::string profileName) {
  LogStore::info("[System::loadLanProfile] loading network profile: " +
                 profileName);
  std::string ssid =
      ConfigStore::configContent()["networkProfiles"][profileName]["ssid"];
  std::string pwd =
      ConfigStore::configContent()["networkProfiles"][profileName]["pwd"];
  std::string staticIp =
      ConfigStore::configContent()["networkProfiles"][profileName]["staticIP"];
  LogStore::dbg("[System::loadLanProfile] SSID: " + ssid);
  if (staticIp.length() && staticIp != "null") {
    std::string gateway =
        ConfigStore::configContent()["networkProfiles"][profileName]["gateway"];
    std::string subnet =
        ConfigStore::configContent()["networkProfiles"][profileName]["subnet"];
    std::string primaryDns =
        ConfigStore::configContent()["networkProfiles"][profileName]
                                    ["primaryDNS"];
    std::string secondaryDns =
        ConfigStore::configContent()["networkProfiles"][profileName]
                                    ["secondaryDNS"];
    staticIpConfig(staticIp, gateway, subnet, primaryDns, secondaryDns);
  }
  activateWifiSTA(ssid, pwd);
  // // NOT WORKING
  // JsonObject networkProfiles =
  // ConfigStore::configContent()["networkProfiles"]; if
  // (!networkProfiles.isNull()) {
  //   LogStore::dbg("[System::loadLanProfile] not null");
  //   JsonObject profileContent = networkProfiles[profileName];
  //   std::string contentDump = "";
  //   serializeJsonPretty(networkProfiles, contentDump);
  //   LogStore::dbg("[System::loadLanProfile] dump content " +
  //   contentDump);
  // } else {
  //   LogStore::info(
  //       "[System::loadLanProfile] couldn't read network profiles");
  // }
}

void System::staticIpConfig(std::string sStaticIP, std::string sGateway,
                            std::string sSubnet, std::string sDns1,
                            std::string sDns2) {
  LogStore::info("[System::staticIpConfig] IP LAN settings : " + sStaticIP +
                 ", gateway: " + sGateway + ", subnet: " + sSubnet +
                 ", dns1: " + sDns1 + ", dns2: " + sDns2);
  std::vector<int> viStaticIP =
      strVectToIntVect(splitFromCharDelim(sStaticIP, '.'));
  std::vector<int> viGateway =
      strVectToIntVect(splitFromCharDelim(sGateway, '.'));
  std::vector<int> viSubnet =
      strVectToIntVect(splitFromCharDelim(sSubnet, '.'));
  std::vector<int> viDns1 = strVectToIntVect(splitFromCharDelim(sDns1, '.'));
  std::vector<int> viDns2 = strVectToIntVect(splitFromCharDelim(sDns2, '.'));

  IPAddress ip(viStaticIP.at(0), viStaticIP.at(1), viStaticIP.at(2),
               viStaticIP.at(3));
  IPAddress gateway(viGateway.at(0), viGateway.at(1), viGateway.at(2),
                    viGateway.at(3));
  IPAddress subnet(viSubnet.at(0), viSubnet.at(1), viSubnet.at(2),
                   viSubnet.at(3));
  // optional
  IPAddress dns1(viDns1.at(0), viDns1.at(1), viDns1.at(2), viDns1.at(3));
  IPAddress dns2(viDns2.at(0), viDns2.at(1), viDns2.at(2), viDns2.at(3));
  // Configures static IP address
  if (!WiFi.config(ip, gateway, subnet, dns1, dns2)) {
    LogStore::info(
        "[System::staticIpConfig] Failed to configure static IP address");
  }
}

// turn on wifi client
void System::activateWifiSTA(std::string ssid, std::string pwd) {
  int retryCounter = 10;
  LogStore::info("[System::activateWifiSTA] Connecting to " +
                 std::string(ssid.c_str()));
  // any wifi conf must be called prior to this
  WiFi.begin(ssid.c_str(), pwd.c_str());

  // wait for connection
  while (retryCounter > 0 && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCounter--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    std::string ipAddr(WiFi.localIP().toString().c_str());
    LogStore::info("[System::activateWifiSTA] Connected with address: " +
                   ipAddr);
  } else {
    LogStore::info("[System::activateWifiSTA] Could not connect to LAN");
  }
}

// turn on wifi access point
void System::activateWifiAP(std::string ssid, std::string pwd) {
  LogStore::info("[System::activateWifiAP] Setting up access point " + ssid);
  WiFi.softAP(ssid.c_str(), pwd.c_str());
  std::string ipAddress(WiFi.softAPIP().toString().c_str());
  LogStore::info("[System::activateWifiAP] Hotspot " + ssid + " available at " +
                 ipAddress);
}
