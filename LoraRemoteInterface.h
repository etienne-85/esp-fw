#include <LoRa.h>
#include <LogStore.h>
#include <RemoteServiceListener.h>
#include <SPI.h>

/**
 * Lora remote service entry
 * incoming LORA msg => RemoteService input
 * RemoteService output => outgoing LORA msg
 */
// define the pins used by the transceiver module
#define ss 10
#define rst 14
#define dio0 2
#define DELAY 5000

int packetsCount = 0;

/**
 * LORA radio/wireless communication protocol
 * Receive Lora message and forward to corresponding service
 */
class LoraRemoteInterface {

public:
  static LoraRemoteInterface &instance();
  void init();
  // void configure(int reg, int val);
  void send(std::string msg);
  void listen();
  void filterMessage(std::string incomingMsg);
  void onMessage(std::string incomingMsg);
};

LoraRemoteInterface &LoraRemoteInterface::instance() {
  // check if service already exists, otherwise create one
  static LoraRemoteInterface singleton;
  return singleton;
}

void LoraRemoteInterface::init() {
  LogStore::dbg("[LoraRemoteInterface::init] MOSI: " + std::to_string(MOSI) +
                 " MISO: " + std::to_string(MISO) +
                 " SCK: " + std::to_string(SCK) + " SS: " + std::to_string(SS));

  // setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  // replace the LoRa.begin(---E-) argument with your location's frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa
  // transceivers ranges from 0-0xFF
  LoRa.setSyncWord(0x64);
  LoRa.setSpreadingFactor(12);
  LogStore::info("[LoraRemoteInterface::init] LORA interface available");
}

void LoraRemoteInterface::send(std::string outgoingMsg) {
  LogStore::dbg("[LoraRemoteInterface::send] Sending packet #" +
                 std::to_string(packetsCount));

  // Send LoRa packet to recipient
  LoRa.beginPacket();
  LoRa.print(outgoingMsg.c_str());
  LoRa.endPacket();
  LogStore::info("[LoraRemoteInterface::send] SENT packet ");
  LogStore::dbg(outgoingMsg);
}

void LoraRemoteInterface::listen() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  std::string incomingMsg("");

  if (packetSize) {
    packetsCount++;
    // received a packet
    LogStore::info("[LoraRemoteInterface::listen] RECEIVED packet");
    // read packet
    while (LoRa.available()) {
      incomingMsg = LoRa.readString().c_str();
      LogStore::dbg(incomingMsg);
    }
    filterMessage(incomingMsg);
  }
}

void LoraRemoteInterface::filterMessage(std::string incomingMsg) {
  JsonDocument root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);
  // root level props common to all services
  // std::string sender = root["src"];
  std::string recipient = root["dst"];
  std::string deviceId = ConfigStore::setting("deviceId");
  if (recipient == deviceId) {
    onMessage(incomingMsg);
  } else {
    LogStore::info(
        "[LoraRemoteInterface::filterMessage] reject message addressed to " +
        recipient + " received on " + deviceId);
  }
}

void LoraRemoteInterface::onMessage(std::string incomingMsg) {
  // service dispatching
  std::string serviceOutput = RemoteServiceListener::dispatchMsg(incomingMsg);
  if (serviceOutput.length() > 0) {
    // LogStore::info("[RemoteService::onMessage] sending reply: " +
    // outgoingMsg);
    send(serviceOutput);
  } else {
    // LogStore::info(
    //     "[LoraRemoteInterface::onMessage] empty message => no reply sent");
  }
}

// void LoraRemoteInterface::configure(int reg, int val) {
//   LogStore::info("[LoraRemoteInterface::configure] set register " +
//                  std::to_string(reg) + " to value " + std::to_string(val));
//   LoRa.writeRegister(reg, val);
// }

#undef ss 10
#undef rst 14
#undef dio0 2
#undef DELAY 5000