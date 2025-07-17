#pragma once
#include <ApiModule.h>
#include <ArduinoJson.h>
#include <CommonObj.h>
#include <LoRa.h>
#include <LinkInterface.h>
// #include <SPI.h>

/**
 * LORA radio/wireless communication interface
 * suitable for long range communication between devices
 */
class LoraInterface : public LinkInterface, public ApiModule {

public:
  static LoraInterface &instance();
  LoraInterface();
  void init();
  void setup(std::string objContent);
  void autoAdjustSF();
  // void configure(int reg, int val);
  void sendText(std::string outgoingMsg);
  void notifyClient(std::string notif);
  void listen();
  std::string onApiCall(Packet &msg);
};
