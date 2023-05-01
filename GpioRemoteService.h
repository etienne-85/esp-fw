/**
 * Web socket service providing remote GPIO control
 * Usage example from browser:
 * let ws = new WebSocket(`ws://${window.location.hostname}/gpio`)
 * let gpio = { pin: 15, pwmChan: 0, val: 18, type: 0 };
 * let msg = {gpios: [gpio]};
 * webSocket.send(JSON.stringify(msg))
 */

#include <ArduinoJson.h>
#include <WebSocketListener.h>
#include <cstddef>
#include <map>
#include <string>
#include <defaults.h>

#define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

enum GPIO_ACTIONS { READ, WRITE, INIT };

enum GPIO_TYPES { ANALOG_PIN, PWM_PIN };

struct GPIO_STATE {
  int val;
  int freq;
  int res;
  int pwmChannel;
  GPIO_TYPES type;
};



class Gpio {
public:
  static std::map<int, Gpio*> pins;
  // static Gpio *getPin(int pin);
  static Gpio *getPinInstance(int pin);
  static Gpio *createPinInstance(int pin, bool isPwm);
  // either create new or update existing from json pin config
  static void parseStatic(std::string pinConf);

public:
  int pin;

  Gpio(int pin);

  virtual int read();

  virtual void write(int val);
  /*
  * Parse (instance)
  * import pin configuration
  */
  virtual void parse(std::string pinConf);

};

class PwmPin : Gpio {
public:
  int dutyCycle = 0;
  int freq = 10000;// pwm freq
  int res = 8;     // pwm res
  int channel = 0; // pwm channel

  PwmPin(int pin)
      : Gpio(pin) {
    init();
  }

  void init();

  virtual void write(int dutyCycle);

  virtual void parse(std::string pinConfStr);

  std::string exportAsJson();
};

class GpioRemoteService : WebSocketListener {
protected:
  // making service available at /gpio
  GpioRemoteService(): WebSocketListener("/gpio"){};
  // filterMsg()
public:
  // overidding base class default method
  std::string processMsg(std::string rawMsg);

  static GpioRemoteService &instance();
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
};
