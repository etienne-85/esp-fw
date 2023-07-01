#include <ArduinoJson.h>
#include <GpioPin.h>
#include <defaults.h>
#include <map>
#include <string>
// #include <miscUtils.h>
#define JSON_SIZE DEFAULT_JSON_SIZE

/*
 *  Static class to manage GPIO pins: find, alloc, free
 */

class GpioFactory {
public:
  static std::map<int, GpioPin *> pins;
  // static Gpio *getPin(int pin);
  static GpioPin *pinFind(int pin);
  static GpioPin *pinAlloc(int pin, std::string pinData);
  static void pinFree(int pin);
  static int pinAuto(int pin, std::string pinData);
  static void resetPinsDefaults();
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/

std::map<int, GpioPin *> GpioFactory::pins;

/*
 *   Find pin from registered instance
 */
GpioPin *GpioFactory::pinFind(int pin) {
  auto pinMatch = GpioFactory::pins.find(pin);
  return pinMatch != GpioFactory::pins.end() ? pinMatch->second
                                             : NULL; // new Gpio(pin);
}

/**
 *  Allocating hardware pin [Static Level]
 * - existing => skip
 * - missing => calling specific pin constructor
 */
GpioPin *GpioFactory::pinAlloc(int pin, std::string pinData) {
  // std::cout << "[GpioFactory::pinAlloc] raw pin data: "<< pinData <<
  // std::endl;
  // std::cout << "[GpioFactory::pinAlloc] pin: "<< pin << std::endl;
  GpioPin *pinInstance = GpioFactory::pinFind(pin);
  StaticJsonDocument<JSON_SIZE> pinRoot;
  // unpack json pin config
  deserializeJson(pinRoot, pinData);

  if (pinInstance == NULL) {
    // std::cout << "[GpioFactory::pinAlloc] create instance for pin "<< pin <<
    // std::endl;
    pinInstance =
        pinRoot["type"] == 1
            ? (GpioPin *)(new GpioPwmPin(pin, pinRoot["chan"], pinRoot["freq"],
                                         pinRoot["res"]))
            : new GpioPin(pin);
    GpioFactory::pins.insert({pin, pinInstance});
    std::cout << "[GpioFactory::pinAlloc] Total registered gpios: "
              << GpioFactory::pins.size() << std::endl;
  } else {
    std::cout << "pin #" << pin << " already allocated " << std::endl;
  }

  return pinInstance;
}

/*
 *  Pin auto mode
 *   - auto alloc pin if missing pin and init conf provided
 *   - auto pin operation: read or write depending on pin internal mode
 *
 *  Returns:
 *   - fail: -1
 *   - current pin value (input/read mode)
 *   - ? (output/write mode)
 */
// TODO: check which value should be returned: instance creation confirmation /
// error? read value?
int GpioFactory::pinAuto(int pin, std::string sPinData) {
  int retVal = -1;
  GpioPin *pinInstance = GpioFactory::pinFind(pin);

  if (pinInstance == NULL) {
    // tries to create pin instance
    // if fail log error: missing init parameters
    pinInstance = GpioFactory::pinAlloc(pin, sPinData);
  }

  if (pinInstance != NULL) {
    StaticJsonDocument<JSON_SIZE> pinRoot;
    // unpack json pin config
    deserializeJson(pinRoot, sPinData);
    int val = pinRoot["val"];
    retVal = pinInstance->autoMode(val);
  }
  // int pinVal = pinInstance !== NULL? pinInstance->autoMode(val):-1;
  return retVal;
}

/**
 * Free hardware pin [Static Level]
 * - existing => calling specific pin destructor
 * - missing => skip
 */
void GpioFactory::pinFree(int pin) {
  // TODO call destructor to free pin
}

/**
 * Watch pins for auto reset
 */
void GpioFactory::resetPinsDefaults() {
  for (auto const &pin : GpioFactory::pins) {
    GpioPin *pinInstance = pin.second;
    pinInstance->reset();
  }
}