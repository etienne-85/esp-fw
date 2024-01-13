/**
 * Web socket service providing remote GPIO control
 * Usage example from browser:
 * let ws = new WebSocket(`ws://${window.location.hostname}/gpio`)
 * let gpio = { pin: 15, pwmChan: 0, val: 18, type: 0 };
 * let msg = {gpios: [gpio]};
 * webSocket.send(JSON.stringify(msg))
 */

#include <ArduinoJson.h>
#include <GpioFactory.h>
#include <GpioPin.h>
#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <WebServer.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>

// #define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

// The HTTPS Server comes in a separate namespace. For easier use, include it
// here.
using namespace httpsserver;

/**
 *   GpioRemoteService providing nterface between websockets and GPIOs:
 *   <WS MSG> => [GpioRemoteService::] => [GpioFactory::] => (GpioPin)
 */
/*
 * All connected client to the service have dedicated ws handler
 * but share same service
 */
class GpioRemoteService : public WebsocketHandler {
  static std::map<int, GpioRemoteService *> instances;
  static WebsocketNode *webSocketNode;
  // this will be called each time new client connects to the /gpio websocket
  static WebsocketHandler *onCreate();
  // for how long pins are maintained in state before restored to default value
  // '0' disables restoring default pin state
  static int resetCycles;
  // internal cycle counter used for pin reset and benchmark
  static int cyclesCount;

public:
  static void init();
  static void loop();

  // =  new WebsocketNode("/gpio", &GpioRemoteService::instancePtr);

private:
  GpioRemoteService(int clientId);
  int clientId = 0;

  // making service available at /gpio
  // GpioRemoteService() : WebsocketNode("/chat", &GpioRemoteService::create);
  // GpioRemoteService(): WebsocketNode("/gpio", &GpioRemoteService::instance);
  // static GpioRemoteService &instance();
  // static WebsocketHandler *instancePtr();

public:
  // This method is called when a message arrives in standalone mode
  void onMessage(WebsocketInputStreambuf *input);
  // Handler function on connection close
  void onClose();
  // overiding default base class method
  void extractMsg(std::string rawMsg);
  // Reply to a ping message sent from client
  void pong(std::string msg);
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<int, GpioRemoteService *> GpioRemoteService::instances;
int GpioRemoteService::resetCycles(0);
int GpioRemoteService::cyclesCount(0);

WebsocketNode *GpioRemoteService::webSocketNode =
    new WebsocketNode("/gpio", &GpioRemoteService::onCreate);

// each time new client connects to the /gpio websocket a new service
// instance is created
WebsocketHandler *GpioRemoteService::onCreate() {
  int clientNb = GpioRemoteService::instances.size();
  GpioRemoteService *instance = NULL;
  if (GpioRemoteService::instances.size() < MAX_CLIENTS) {
    instance = new GpioRemoteService(clientNb);
    GpioRemoteService::instances.insert({clientNb, instance});
  } else {
    LogStore::info(
        "[GpioRemoteService::onCreate] max number of client reached #" +
        std::to_string(clientNb));
  }
  // will be cast to WebsocketHandler*
  return instance;
}

// GpioRemoteService &GpioRemoteService::instance() {
//   static GpioRemoteService singleton;
//   return singleton;
// }

// WebsocketHandler *GpioRemoteService::instancePtr() {
//   return &GpioRemoteService::instance();
// }

GpioRemoteService::GpioRemoteService(int clientId)
    : WebsocketHandler(), clientId(clientId) {
  LogStore::info("[GpioRemoteService] Client #" + std::to_string(clientId) +
                 " connected");
}

void GpioRemoteService::init() {
  LogStore::info("[GpioRemoteService::init]");
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

void GpioRemoteService::loop() {
  if (GpioRemoteService::resetCycles == 0 ||
      GpioRemoteService::cyclesCount <= GpioRemoteService::resetCycles) {
    GpioRemoteService::cyclesCount++;
  } else {
    // if no keep alive signal received after a while
    GpioFactory::resetPinsDefaults();
  }
}

// When the websocket is closing, we remove the client from the array
void GpioRemoteService::onClose() {
  LogStore::info("[GpioRemoteService] Client #" + std::to_string(clientId) +
                 " disconnected");
  // for(int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] == this) {
  //     activeClients[i] = nullptr;
  //   }
  // }
}

/*
 * pin operation requirements
 * required  |  instance exists | Static/Instance level  |  pinData | auto mode
 * ----------+------------------+------------------------+----------+--------------------------+
 * alloc               N                 S                     x       pin conf
 * provided read                Y                 I pin instance exists write Y
 * I                     x       pin instance exists free                Y S
 * dump                Y                 I
 */
// unpack or extract data from message
// Finally, passing messages around. If we receive something, we send it to all
// other clients
void GpioRemoteService::onMessage(WebsocketInputStreambuf *inbuf) {
  // Get the input message
  std::ostringstream ss;
  std::string msg;
  ss << inbuf;
  msg = ss.str();

  extractMsg(msg);
}

void GpioRemoteService::extractMsg(std::string incomingMsg) {
  // std::cout << "[GpioRemoteService::unpackMsg] Incoming message " <<
  // incomingMsg
  //           << std::endl;
  StaticJsonDocument<JSON_MSG_SIZE> root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);

  // root level props
  std::string cmd = root["cmd"];   // for operation not tied to any pin
  int msgRefId = root["msgRefId"]; // in case reply is needed
  JsonObject jsPinsBatch = root["pinsBatch"];
  LogStore::info("[GpioRemoteService::extractMsg] msgRefID " +
                 std::to_string(msgRefId));

  // JsonArray array = jsonMsg["gpios"];
  //     for (JsonVariant gpio : array) {
  //       std::string pinConfig;
  //       serializeJson(gpio, pinConfig);
  //       // Gpio.assignPin(pinConfig)
  //       Gpio::parseAll(pinConfig);
  //     }

  std::string replyMsg("Default reply from GpioRemoteService");

  // Ping signal to:
  // - maintain latest pin state
  // - make sure service is still alive
  // - get cycles count stats to benchmark ESP usage
  if (cmd == "ping") {
    replyMsg = "MSG_ID #" + std::to_string(msgRefId) + " CLI_ID #TODO" +
               " cycles #" + std::to_string(cyclesCount);
    cyclesCount = 0; // will maintain latest pin state
  } else if (cmd == "config") {
    JsonObject config = root["config"];
    resetCycles = config["resetCycles"];
    LogStore::info("[GpioRemoteService::unpackMsg] Pins reset cycle set to " +
                   std::to_string(resetCycles));
  }

  // hash pins batch and passdown pin data to corresponding instance
  // or make static call
  for (JsonPair kv : jsPinsBatch) {
    // key/value
    int pin = std::stoi(kv.key().c_str());
    // extract pin data
    JsonObject jsPinData = kv.value().as<JsonObject>();

    GpioPin *pinInstance = GpioFactory::pinFind(pin);

    // returned value of pin operation if applicable
    std::string *sPinRes = NULL;

    const char *pinOp = jsPinData["op"];
    switch (*pinOp) {
    case 'a': // alloc
    {
      std::string sPinData;
      serializeJson(jsPinData, sPinData);
      pinInstance = GpioFactory::pinAlloc(pin, sPinData); // [static]
      break;
    }
    case 'r': // read
    {
      // Gpio::pinRead(pin);
      std::string sPinVal(std::to_string(pinInstance->read()));
      sPinRes = &sPinVal;
      break;
    }
    case 'w': // write
    {
      // std::cout << "[GpioRemoteService::unpackMsg] pin write " << std::endl;
      int pinVal = jsPinData["val"];
      // Gpio::pinWrite(pin, data);
      pinInstance->write(pinVal);
      break;
    }
    case 'f': // free
    {
      GpioFactory::pinFree(pin); // [static]
      break;
    }
    case 'd': // dump
    {
      // Gpio::pinDump(pin);
      std::string sPinVal(pinInstance->dump());
      sPinRes = &sPinVal;
      break;
    }
    default: // missing pin op => auto pin action
    {
      LogStore::info("[GpioRemoteService::unpackMsg] no OP provided defaulting "
                     "to pinAuto mode ");
      // pack/encode/serialize for pin data forwarding
      std::string sPinData;
      serializeJson(jsPinData, sPinData);
      int retVal = GpioFactory::pinAuto(pin, sPinData);
      if (retVal == -1) {
        LogStore::info("FAILED auto mode for pin " + std::to_string(pin));
      }
      std::string sPinVal(std::to_string(retVal));
      sPinRes = &sPinVal;
      // std::string sPinVal(std::to_string());
      // sPinRes = &sPinVal;
    }
    }
  }
  // optional reply depending on pin operation
  // if (msgRefId) {
  //   std::string replyMsg("TODO");
  // }
  if (replyMsg.length()) {
    String dataOut(replyMsg.c_str());
    // webSocket.textAll(dataOut);
  }
}

void GpioRemoteService::pong(std::string msg) {
  LogStore::info("[GpioRemoteService] received message: " + msg);

  // Send it back to every client
  // for (int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] != nullptr) {
  //     activeClients[i]->send(msg, SEND_TYPE_TEXT);
  //   }
  // }
  this->send(msg, SEND_TYPE_TEXT);
}

/******************
 *** DEPRECATED ***
 ******************/

/**
 * isWriteMode: write or read
 * isPWM: pwm
 */
// void assignGpio(int pin, bool isWriteMode, bool isPWM) {

//   std::cout << "[GpioRemoteService] pin #" << pin << " registered listener "
//             << std::endl;

//   // initialized?
//   // if (!gpioStateMap.contains(pin)) {
//   if (gpioStateMap->find(pin) == gpioStateMap->end()) {
//     // init gpio
//     Serial.println("Init as PWM ");

//     if (isPWM) {
//       GPIO_STATE pinState;
//       pinState.val = 0, pinState.freq = 10000, pinState.res = 8;
//       pinState.pwmChannel = gpio["pwmChan"];
//       gpioStateMap->insert({pin, pinState});
//       Serial.print("gpioStateMap size is now: ");
//       Serial.println(gpioStateMap->size());
//       Serial.print("configuring pin ");
//       pinMode(pin, OUTPUT);
//       ledcSetup(pinState.pwmChannel, pinState.freq, pinState.res);
//       ledcAttachPin(pin, pinState.pwmChannel);
//     }
//   }
//   if (isPWM && isWriteMode) {
//   }
// }

/*
*  TOP LEVEL CMD:
* - EXEC: exec pin op (specified or inferred by current pin state)
*   pinData: {pin: data}
* - DUMP: dump multiple pins
*   pinData: {id}
* - FULLDUMP: returns all pin states
* - CHGPIN/CHGSTATE:  change property of specific pin
* - EXEC_BATCH: exec mulitple pins actions (specified by current pin state)
*   batch: [pinData]
pinData: {chmod: INPUT/OUTPUT, chtyp: PWM, override?}
*/

/*
 * PER PIN CRUD OPERATIONS support:
 * - C: construct/allocate :handle pin assignment/allocation
 *   - non existing pin => assign new pin
 *   - existing pin and no override flag => skip
 *   - existing pin + override flag => unassign previous pin and reallocate
 *   option flags: -o --override => D+C
 * - R: read
 *   option flags: -d --dump, -fd --fulldump
 * - U: update (or write)
 * - D: destruct/free
 */

/*
 * READ
 * - existing => read pin value
 * - missing => skip
 */
// int Gpio::pinRead(int pin) {
//   Gpio *pinInstance = Gpio::findPinInstance(pin)

//   if(pinInstance == NULL)
//     std::cout << "[Gpio::parseAll] operation not allowed, pin "<< pin <<"
//     must be allocated first "<< std::endl;
//   return pinInstance != NULL? pinInstance->read(): NULL
// }

/**
 * WRITE
 * - existing => write pin value
 * - missing => skip
 */
// bool Gpio::pinWrite(int pin, int val) {
//   Gpio *pinInstance = Gpio::findPinInstance(pin)
//   if(pinInstance == NULL)
//     std::cout << "[Gpio::parseAll] operation not allowed, pin "<< pin <<"
//     must be allocated first "<< std::endl;
//   return pinInstance !== NULL? pinInstance->write(val): false
// }