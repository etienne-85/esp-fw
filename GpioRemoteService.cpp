#include <GpioRemoteService.h>
#include <iostream>

/**
*   PIN
*/

std::map<int, Gpio*> Gpio::pins;

Gpio::Gpio(int pin) {
    this->pin = pin;
    Gpio::pins.insert({pin, this});
    std::cout << "[Gpio] create pin #" << pin << std::endl;
    std::cout << "total registered gpios: " << Gpio::pins.size() << std::endl;
  }

int Gpio::read() {
    std::cout << "[Gpio #" << pin << "] [read] " << std::endl;
    return NULL;
  }

void Gpio::write(int val) {
    std::cout << "[Gpio #" << pin << "] [write] " << val << std::endl;
  }

/*
* Call corresponding pin parse config method
* finding/creating pin and calling parse instance
*/
// Static version
void Gpio::parseStatic(std::string pinConfStr) {
  StaticJsonDocument<JSON_MSG_SIZE> pinConfJson;
  // convert to a json object
  DeserializationError error = deserializeJson(pinConfJson, pinConfStr);
  bool isPwmType = true; // TODO: replace by pinConfJson["type"]
  int pin = pinConfJson["pin"];
  std::cout << "[Gpio::parseStatic] process pin# " << pin << std::endl;
  Gpio *pinInstance = Gpio::getPinInstance(pin) != NULL? Gpio::getPinInstance(pin) : Gpio::createPinInstance(pin, isPwmType);
  // this will call pin parse instance
  pinInstance->parse(pinConfStr);
  // GPIO_ACTIONS action = gpio["action"];
}

Gpio *Gpio::getPinInstance(int pin) {
  auto pinMatch = Gpio::pins.find(pin);
  return pinMatch != Gpio::pins.end()? pinMatch->second : NULL; //new Gpio(pin);
}

Gpio *Gpio::createPinInstance(int pin, bool isPwm) {
  return isPwm ?(Gpio*) (new PwmPin(pin)): new Gpio(pin);
}

void Gpio::parse(std::string pinConfStr) {
    std::cout << "[Gpio::parse] unimplemented method "<< std::endl;
  }

/**
*   PWM PIN
*/

void PwmPin::init() {
    std::cout << "[PwmPin #" << pin << "] [init] " << std::endl;
    pinMode(pin, OUTPUT);
    ledcSetup(channel, freq, res);
    ledcAttachPin(pin, channel);
  }

void PwmPin::write(int dutyCycle) {
  std::cout << "[PwmPin #" << pin << "] [write] dutyCycle: " << dutyCycle << std::endl;
  ledcWrite(channel, dutyCycle);
}

void PwmPin::parse(std::string pinConfStr) {
    std::cout << "[PwmPin #" << pin << "] [parseConf] parsing "
              << std::endl;
    StaticJsonDocument<JSON_SIZE> pinConf;

    // parse json string
    DeserializationError error = deserializeJson(pinConf, pinConfStr);

    // Test if parsing succeeds.
    if (!error) {
        dutyCycle = pinConf["val"];
    } else {
      Serial.print(F("deserializeJson() failed: "));
      // Serial.println(error.f_str());
    }
  }

  std::string PwmPin::exportAsJson() {
    // build json object from class properties
    StaticJsonDocument<JSON_SIZE> jsonGpio;

    jsonGpio["pin"] = pin;
    jsonGpio["val"] = dutyCycle;
    jsonGpio["freq"] = freq;
    jsonGpio["res"] = res;
    jsonGpio["chan"] = channel;

    std::string out("");
    // serializeJson(doc, out);
    return out;
  }

/**
*   GpioRemoteService
*/

GpioRemoteService &GpioRemoteService::instance(){
    static GpioRemoteService singleton;
    return singleton;
}

std::string GpioRemoteService::processMsg(std::string rawMsg) {
    Serial.println("[GpioService] incoming message");

    StaticJsonDocument<JSON_MSG_SIZE> jsonMsg;
    // convert to a json object
    DeserializationError error = deserializeJson(jsonMsg, rawMsg);
    // Check parsing was successful.
    if (!error) {
      // extract the values
      JsonArray array = jsonMsg["gpios"];
      for (JsonVariant gpio : array) {
        std::string pinConfig;
        serializeJson(gpio, pinConfig);
        // Gpio.assignPin(pinConfig)
        Gpio::parseStatic(pinConfig);
      }
      std::string replyMsg("default reply");
      return replyMsg;
    }
  }

