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
#include <WebSocketListener.h>
#include <cstddef>
#include <defaults.h>
#include <iostream>
#include <string>

// #define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

/**
 *   GpioRemoteService providing nterface between websockets and GPIOs:
 *   <WS MSG> => [GpioRemoteService::] => [GpioFactory::] => (GpioPin)
 */

class GpioRemoteService : WebSocketListener {
protected:
  // making service available at /gpio
  GpioRemoteService() : WebSocketListener("/gpio"){};
  int resetCycles = 0; // hearbeats delay or after how many cycles pins should
                       // be reset to their default value
  int cyclesCount = 0;

public:
  // overidding base class default method
  std::string unpackMsg(std::string rawMsg);
  void loop();
  static GpioRemoteService &instance();
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/

GpioRemoteService &GpioRemoteService::instance() {
  static GpioRemoteService singleton;
  return singleton;
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
std::string GpioRemoteService::unpackMsg(std::string incomingMsg) {
  // std::cout << "[GpioRemoteService::unpackMsg] Incoming message " <<
  // incomingMsg
  //           << std::endl;
  StaticJsonDocument<JSON_MSG_SIZE> root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);

  // root level props
  std::string cmd = root["cmd"];           // for operation not tied to any pin
  std::string msgRefId = root["msgRefId"]; // in case reply is needed
  JsonObject jsPinsBatch = root["pinsBatch"];
  // std::cout << "[GpioRemoteService::unpackMsg] msgRefID " << msgRefId
  //           << std::endl;

  // JsonArray array = jsonMsg["gpios"];
  //     for (JsonVariant gpio : array) {
  //       std::string pinConfig;
  //       serializeJson(gpio, pinConfig);
  //       // Gpio.assignPin(pinConfig)
  //       Gpio::parseAll(pinConfig);
  //     }

  if (cmd == "heartbeat") {
    // keep last command alive before pin is reset to default value
    cyclesCount = 0;
  } else if (cmd == "config") {
    JsonObject config = root["config"];
    resetCycles = config["resetCycles"];
    std::cout << "[GpioRemoteService::unpackMsg] Pins reset cycle set to "
              << resetCycles << std::endl;
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
      // std::cout << "[GpioRemoteService::unpackMsg] -> pinAlloc " <<
      // std::endl; pack/encode/serialize for pin data forwarding
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
      std::cout << "[GpioRemoteService::unpackMsg] no OP provided defaulting "
                   "to pinAuto mode "
                << std::endl;
      // pack/encode/serialize for pin data forwarding
      std::string sPinData;
      serializeJson(jsPinData, sPinData);
      int retVal = GpioFactory::pinAuto(pin, sPinData);
      if (retVal == -1) {
        std::cout << "FAILED auto mode for pin " << pin << std::endl;
      }
      std::string sPinVal(std::to_string(retVal));
      sPinRes = &sPinVal;
      // std::string sPinVal(std::to_string());
      // sPinRes = &sPinVal;
    }
    }
  }
  // optional reply depending on pin operation
  std::string replyMsg("TODO");
  return replyMsg;
}

void GpioRemoteService::loop() {
  if (cyclesCount <= resetCycles) {
    cyclesCount++;
  } else {
    GpioFactory::resetPinsDefaults();
  }
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