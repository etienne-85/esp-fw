#include <Arduino.h>
#include <GpioPin.h>
#include <LogStore.h>

using namespace std;
/**
 *   BASE PIN
 */

// TODO allocate corresponding hardware pin in constructor
GpioPin::GpioPin(int pin, int defaultValue)
    : pin(pin), defaultValue(defaultValue) {
  // Gpio::pins.insert({pin, this});
  LogStore::info("[GpioPIN#" + to_string(pin) + "::alloc]");
}

// TODO write destructor which will unassign hardware pin

/*
 *   Read pin value
 */
int GpioPin::read() {
  LogStore::info("[GpioPin #" + to_string(pin) + "] [read] ");
  return -1;
}

/*
 *   Write pin value
 */
void GpioPin::write(int val) {
  LogStore::info("[GpioPin #" + to_string(pin) + "] [write] " + to_string(val));
}

/*
 *   Dump pin content
 */
std::string GpioPin::dump() {
  LogStore::info("[GpioPin #" + to_string(pin) + "] [dump] ");
  return NULL;
}

/*
 * Pin auto mode depending on current pin internal state:
 * - input => read
 * - output => write
 */
int GpioPin::autoMode(int val) {
  // TODO
  return -1;
}

/*
 *   Dump pin content
 */
void GpioPin::reset() {
  if (defaultValue != -1) {
    LogStore::info("[GpioPin #" + to_string(pin) + "] [reset] ");
    write(defaultValue);
  }
}

/**
 *   PWM PIN
 */

// #override base class constructor
GpioPwmPin::GpioPwmPin(int pin, int chan, int freq, int res, int defaultValue)
    : GpioPin(pin, defaultValue), channel(chan), freq(freq), res(res) {
  LogStore::info("[PwmPIN#" + std::to_string(pin) + "::alloc] on chan #" +
                 std::to_string(chan) + " (freq:" + std::to_string(freq) +
                 ", res:" + std::to_string(res) + ")");
  // pinMode(pin, OUTPUT);
  ledcSetup(chan, freq, res);
  ledcAttachPin(pin, chan);
}

// #override base class write
void GpioPwmPin::write(int dutyCycle) {
  LogStore::info("[PwmPIN#" + std::to_string(pin) +
                 "::write] dutyCycle: " + std::to_string(dutyCycle));
  this->value = dutyCycle;
  ledcWrite(channel, dutyCycle);
}

// #override base class dump
std::string GpioPwmPin::dump() {
  // build json object from class properties
  // StaticJsonDocument<JSON_SIZE> jsonGpio;

  // jsonGpio["pin"] = pin;
  // jsonGpio["val"] = dutyCycle;
  // jsonGpio["freq"] = freq;
  // jsonGpio["res"] = res;
  // jsonGpio["chan"] = channel;

  // std::string out("");
  // // serializeJson(doc, out);
  // return out;
}
