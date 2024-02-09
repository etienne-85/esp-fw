#pragma once
#include <ArduinoJson.h>
#include <System.h>
#include <MessageInterface.h>
#include <LoRa.h>
#include <MessageListener.h>
#include <SPI.h>

/**
 * LORA radio/wireless communication interface
 * suitable for long range communication between devices
 */
class LoraInterface : public MessageInterface {

public:
  static LoraInterface &instance();
  void init();
  // void configure(int reg, int val);
  void sendText(std::string outgoingMsg);
  void notifyClient(std::string notif);
  void listen();
  void filterMessage(std::string incomingMsg);
  void onMessage(std::string incomingMsg);
};
