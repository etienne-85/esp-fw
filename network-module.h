#include <Arduino.h>
#include <FirmwareModule.h>
#include <WiFi.h>
#include <iostream>
// #include "../../../Dont-Commit-Me.h"
class NetworkModule : public FirmwareModule {

public:
  // if redefining constructor, call base class to register
  NetworkModule() : FirmwareModule("Network") { FirmwareModule::setup(); }

  void setup() {
    Serial.println("[Network::setup] Begin");
    wifiAP();
    wifiSTA();
    Serial.println("[Network::setup] Done");
  }

  void wifiAP() {
    std::string hotspotSSID = ConfigLoader::jsonConfig["hotspotSSID"];
    std::string hotspotPWD = ConfigLoader::jsonConfig["hotspotPWD"];
    std::cout << "[Network::WIFI::AP] Setting up access point " << hotspotSSID  << std::endl;
    WiFi.softAP(hotspotSSID.c_str(), hotspotPWD.c_str());
    std::cout << "[Network::WIFI::AP]" << hotspotSSID << " available at " << WiFi.softAPIP() << std::endl;
  }

  void wifiSTA() {
    std::string wifiSSID = ConfigLoader::jsonConfig["wifiSSID"];
    std::string wifiPWD = ConfigLoader::jsonConfig["wifiPWD"];
    std::cout << "[Network::WIFI::STA] Connecting to " << wifiSSID.c_str()<<std::endl;;
    WiFi.begin(wifiSSID.c_str(), wifiPWD.c_str());

    // wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");

    std::string ipAddr(WiFi.localIP().toString().c_str());
    std::cout << "[Network::WIFI::STA] Connected with address: " << ipAddr << std::endl;
  }
};
