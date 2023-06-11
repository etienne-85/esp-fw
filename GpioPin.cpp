#include <Arduino.h>
#include <GpioPin.h>
/**
 *   BASE PIN
 */

// TODO allocate corresponding hardware pin in constructor
GpioPin::GpioPin(int pin) : pin(pin) {
  // Gpio::pins.insert({pin, this});
  std::cout << "[GpioPin] create pin #" << pin << std::endl;
}

// TODO write destructor which will unassign hardware pin

/*
 *   Read pin value
 */
int GpioPin::read() {
  std::cout << "[GpioPin #" << pin << "] [read] " << std::endl;
  return -1;
}

/*
 *   Write pin value
 */
void GpioPin::write(int val) {
  std::cout << "[GpioPin #" << pin << "] [write] " << val << std::endl;
}

/*
 *   Dump pin content
 */
std::string GpioPin::dump() {
  std::cout << "[GpioPin #" << pin << "] [dump] " << std::endl;
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

/**
 *   PWM PIN
 */

// #override base class constructor
GpioPwmPin::GpioPwmPin(int pin, int chan, int freq, int res)
    : GpioPin(pin), channel(channel), freq(freq), res(res) {
  std::cout << "[PwmPin PIN#" << pin << "] initializing with chan: " << chan
            << ", freq:" << freq << ", res:" << res << std::endl;
  pinMode(pin, OUTPUT);
  ledcSetup(chan, freq, res);
  ledcAttachPin(pin, chan);
}

// #override base class write
void GpioPwmPin::write(int dutyCycle) {
  std::cout << "[PwmPin::write] [PIN#" << pin << "] dutyCycle: " << dutyCycle
            << std::endl;
  this->dutyCycle = dutyCycle;
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
