#include <Arduino.h>
#include <GpioPin.h>
#include <LogStore.h>
#include <iostream>
#include <Notifications.h>

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

/*
 * TriggerPin
 */

template <int pinId> void onPinStateChange(void) {
  std::string evtType = EventTypeMap[EventType::PIN_TRIGGER];
  std::string evtData =
      "[TriggerPin::onPinStateChange] " + std::to_string(pinId);
  // EventContext evtCtx;  // use default, customise any required
  // evtCtx.origin = EventOrigin.LOCAL;
  // evtCtx.timestamp = 0;
  // evtCtx.priority = 0;
  Event evt;
  evt.type = evtType;
  evt.content = evtData;
  // evt.context = evtCtx;
  EventQueue::pushEvent(evt);
}

// Create a map of onPinStateChange callbacks
// std::map<int, int> PinInterruptCallbackMap{
//     {0, onPinStateChange<0>}, {1, onPinStateChange<1>},
//     {2, onPinStateChange<2>}, {3, onPinStateChange<3>},
//     {4, onPinStateChange<4>}, {5, onPinStateChange<5>}
//     };
// Create a map of three (string, int) pairs
// std::map<int, void(*) void> m{{1, &onPinStateChange<1>},
//                               {2, &onPinStateChange<2>},
//                               {3, &onPinStateChange<3>}};

void (*pinInterruptCallbackMap[48])(void) = {
    &onPinStateChange<1>,  &onPinStateChange<2>,  &onPinStateChange<3>,
    &onPinStateChange<4>,  &onPinStateChange<5>,  &onPinStateChange<6>,
    &onPinStateChange<7>,  &onPinStateChange<8>,  &onPinStateChange<9>,
    &onPinStateChange<10>, &onPinStateChange<11>, &onPinStateChange<12>,
    &onPinStateChange<13>, &onPinStateChange<14>, &onPinStateChange<15>,
    &onPinStateChange<16>, &onPinStateChange<17>, &onPinStateChange<18>,
    &onPinStateChange<19>, &onPinStateChange<20>, &onPinStateChange<21>,
    &onPinStateChange<22>, &onPinStateChange<23>, &onPinStateChange<24>,
    &onPinStateChange<25>, &onPinStateChange<26>, &onPinStateChange<27>,
    &onPinStateChange<28>, &onPinStateChange<29>, &onPinStateChange<30>,
    &onPinStateChange<31>, &onPinStateChange<32>, &onPinStateChange<33>,
    &onPinStateChange<34>, &onPinStateChange<35>, &onPinStateChange<36>,
    &onPinStateChange<37>, &onPinStateChange<38>, &onPinStateChange<39>,
    &onPinStateChange<40>, &onPinStateChange<41>, &onPinStateChange<42>,
    &onPinStateChange<43>, &onPinStateChange<44>, &onPinStateChange<45>,
    &onPinStateChange<46>, &onPinStateChange<47>, &onPinStateChange<48>};

TriggerPin::TriggerPin(int pinNb, bool defaultLow)
    : GpioPin(pinNb, defaultLow ? LOW : HIGH) {
  LogStore::info("[TriggerPin#" + std::to_string(pinNb) + "::assign] ");
  /// PIR Motion Sensor mode INPUT_PULLUP
  pinMode(pinNb, INPUT_PULLDOWN);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING
  // mode
  attachInterrupt(digitalPinToInterrupt(pinNb),
                  pinInterruptCallbackMap[pinNb - 1], RISING);
}

/*
 * Digital output pin
 */

DigitalOutputPin::DigitalOutputPin(int pinNb, bool defaultPinState)
    : GpioPin(pinNb, defaultPinState ? HIGH : LOW) {
  LogStore::info("[DigitalOutputPin#" + std::to_string(pinNb) + "::assign] ");
  // set the digital pin as output:
  pinMode(pinNb, OUTPUT);
}

void DigitalOutputPin::write(bool pinState) {
  digitalWrite(pin, pinState ? HIGH : LOW);
}
