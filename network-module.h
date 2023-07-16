#include <Arduino.h>
#include <FirmwareModule.h>
#include <WiFi.h>
// #include "../../../Dont-Commit-Me.h"
class NetworkModule : public FirmwareModule {

public:
  // if redefining constructor, call base class to register
  NetworkModule() : FirmwareModule("Network") { FirmwareModule::setup(); }

  void setup() {
    LogStore::info("[Network::setup] Begin");
    wifiAP();
    wifiSTA();
    LogStore::info("[Network::setup] Done");
  }

  void wifiAP() {
    std::string hotspotSSID = ConfigLoader::jsonConfig["hotspotSSID"];
    std::string hotspotPWD = ConfigLoader::jsonConfig["hotspotPWD"];
    LogStore::info("[Network::WIFI::AP] Setting up access point " +
                   hotspotSSID);
    WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
    LogStore::info("[Network::WIFI::AP]" + hotspotSSID + " available at " +
                   std::to_string(WiFi.softAPIP()));
  }

  void wifiSTA() {
    std::string wifiSSID = ConfigLoader::jsonConfig["wifiSSID"];
    std::string wifiPWD = ConfigLoader::jsonConfig["wifiPWD"];
    LogStore::info("[Network::WIFI::STA] Connecting to " +
                   std::string(wifiSSID.c_str()));
    WiFi.begin(wifiSSID.c_str(), wifiPWD.c_str());

    // wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");

    std::string ipAddr(WiFi.localIP().toString().c_str());
    LogStore::info("[Network::WIFI::STA] Connected with address: " + ipAddr);
  }
};
