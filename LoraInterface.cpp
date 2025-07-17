#include <LogStore.h>
#include <LoraInterface.h>
#include <Notifications.h>
#define API_MODULE "Lora"
// define the pins used by the transceiver module
#define ss 10
#define rst 14
#define dio0 2
#define DELAY 5000

int packetsCount = 0;
LoraInterface::LoraInterface()
    : LinkInterface(LinkInterfaceType::LORA), ApiModule(API_MODULE) {}

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

/*
* callable remotely to adjust SF
 decrease value from highest to lower values
*/
void LoraInterface::autoAdjustSF() {
  // try sending message at SF value
  // SUCCESS => retry with lower SF value
  // FAIL => retry with higher SF, retain successful value +1
}

void LoraInterface::sendText(std::string outgoingMsg) {
  // Send LoRa packet to recipient
  LoRa.beginPacket();
  LoRa.print(outgoingMsg.c_str());
  LoRa.endPacket();
  LogStore::info("[LoraInterface::send] SENT packet");
  LogStore::dbg(outgoingMsg);
  SndNotif::instance().bips(1, 32, 20);
}

void LoraInterface::notifyClient(std::string msgContent) {
  sendText(msgContent);
}

void LoraInterface::listen() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  std::string incomingMsg("");

  if (packetSize) {
    packetsCount++;
    // read packet
    while (LoRa.available()) {
      incomingMsg = LoRa.readString().c_str();
      // received a packet
      LogStore::info("[LoraInterface::listen] RECEIVED packet");
      LogStore::dbg(incomingMsg);
    }
    SndNotif::instance().bips(2, 32, 100);
    onPacket(incomingMsg);
  }
}

std::string LoraInterface::onApiCall(Packet &msg) {
  LogStore::info("[LoraInterface::onApiCall] " + msg.cmd);

  // std::string apiCommand = apiInput["cmd"];
  if (msg.cmd == "setup") {
    setup(msg.cmd);
  }
  return "";
}

void LoraInterface::setup(std::string objContent) {
  JsonDocument settings;
  // convert to a json object
  DeserializationError error = deserializeJson(settings, objContent);
  if (settings["SF"]) {
    int sf = settings["SF"];
    if (sf >= 7 && sf <= 12) {
      LogStore::info("[LoraInterface::setup] set spreading factor SF to " +
                     std::to_string(sf));
      LoRa.setSpreadingFactor(sf);
    } else {
      LogStore::info("[LoraInterface::setup] Invalid SF value: " +
                     std::to_string(sf));
    }
  }
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
#undef API_MODULE