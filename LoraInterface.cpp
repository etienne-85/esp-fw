#include <LogStore.h>
#include <LoraInterface.h>
// define the pins used by the transceiver module
#define ss 10
#define rst 14
#define dio0 2
#define DELAY 5000

int packetsCount = 0;

LoraInterface &LoraInterface::instance() {
  // check if service already exists, otherwise create one
  static LoraInterface singleton;
  return singleton;
}

void LoraInterface::init() {
  LogStore::dbg("[LoraInterface::init] MOSI: " + std::to_string(MOSI) +
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
  LogStore::info("[LoraInterface::init] LORA interface available");
}

void LoraInterface::sendText(std::string outgoingMsg) {
  LogStore::dbg("[LoraInterface::send] Sending packet #" +
                std::to_string(packetsCount));

  // Send LoRa packet to recipient
  LoRa.beginPacket();
  LoRa.print(outgoingMsg.c_str());
  LoRa.endPacket();
  LogStore::info("[LoraInterface::send] SENT packet ");
  LogStore::dbg(outgoingMsg);
}

void LoraInterface::notifyClient(std::string notif) {
  std::string msg("");
  sendText(msg);
}

void LoraInterface::listen() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  std::string incomingMsg("");

  if (packetSize) {
    packetsCount++;
    // received a packet
    LogStore::info("[LoraInterface::listen] RECEIVED packet");
    // read packet
    while (LoRa.available()) {
      incomingMsg = LoRa.readString().c_str();
      LogStore::dbg(incomingMsg);
    }
    filterMessage(incomingMsg);
  }
}

void LoraInterface::filterMessage(std::string incomingMsg) {
  JsonDocument root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);
  // root level props common to all services
  // std::string sender = root["src"];
  std::string recipient = root["dst"];
  std::string deviceId = System::cfgStore.configContent()["deviceId"];
  if (recipient == deviceId) {
    onMessage(incomingMsg);
  } else {
    LogStore::info(
        "[LoraInterface::filterMessage] reject message addressed to " +
        recipient + " received on " + deviceId);
  }
}

std::string LoraInterface::onMessage(std::string incoming) {
  // service dispatching
  std::string outgoing = MessageInterface::onMessage(incoming);
  // ogStore::dbg("[WsInterface::onMessage] message received from client " +
  //              clientKey + ": " + incoming);
  // ApiCall *apiCall = ApiCall::fromMinifiedMsg(incoming);
  // std::string outgoing = ApiModule::dispatchApiCall(apiCall);
  if (outgoing.length() > 0) {
    // LogStore::info("[RemoteService::onMessage] sending reply: " +
    // outgoingMsg);
    sendText(outgoing);
  } else {
    // LogStore::info(
    //     "[LoraInterface::onMessage] empty message => no reply sent");
  }
  return "";
}

// void LoraInterface::configure(int reg, int val) {
//   LogStore::info("[LoraInterface::configure] set register " +
//                  std::to_string(reg) + " to value " + std::to_string(val));
//   LoRa.writeRegister(reg, val);
// }

#undef ss
#undef rst
#undef dio0
#undef DELAY