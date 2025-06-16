#include <ArduinoJson.h>
#include <GpioPin.h>
#include <map>
#include <string>
// #include <miscUtils.h>

/*
 *  Static class to manage GPIO pins: find, assign, free
 */

class GpioFactory {
public:
  static std::map<int, GpioPin *> pins;
  // static Gpio *getPin(int pin);
  static GpioPin *pinFind(int pin);
  static GpioPin *pinAssign(int pin, std::string pinData);
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
GpioPin *GpioFactory::pinAssign(int pin, std::string pinData) {
  // std::cout << "[GpioFactory::pinAssign] raw pin data: "<< pinData <<
  // std::endl;
  // std::cout << "[GpioFactory::pinAssign] pin: "<< pin << std::endl;
  GpioPin *pinInstance = GpioFactory::pinFind(pin);
  JsonDocument pinRoot;
  // unpack json pin config
  deserializeJson(pinRoot, pinData);

  if (pinInstance == NULL) {
    // std::cout << "[GpioFactory::pinAssign] create instance for pin "<< pin <<
    // std::endl;
    pinInstance =
        pinRoot["type"] == 1
            ? (GpioPin *)(new GpioPwmPin(pin, pinRoot["chan"], pinRoot["freq"],
                                         pinRoot["res"], pinRoot["default"]))
            : new GpioPin(pin, pinRoot["default"]);
    GpioFactory::pins.insert({pin, pinInstance});
    std::cout << "[GpioFactory::pinAssign] Total registered gpios: "
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
    pinInstance = GpioFactory::pinAssign(pin, sPinData);
  }

  if (pinInstance != NULL) {
    JsonDocument pinRoot;
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