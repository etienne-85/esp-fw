#pragma once
#include <string>

/*
 *   PIN model, last level before hardware implementing GPIO's
 *.  basic operation: read, write, dump
 *  OUTPUT PINS:
    - Digital: static LOW/HIGH discrete state
    - PWM: LOW/HIGH discrete state varying at some frequency
    INPUT PINS:
    - Digital: HIGH or LOW state
    - Analog: continuous value
 */

enum PIN_TYPE {
  DIP, // Digital input
  DOP, // Digital output
  PWM, // Pulse width modulated output
  AIP, // Analog input
  TDIP // Trigger digital input pin
};

/*
 *   BASE PIN
 */
class GpioPin {
  // public:
  //   static std::map<int, GpioPin*> pins;
  //   static GpioPin *getPinInstance(int pin);
  //   static GpioPin *createPinInstance(int pin, bool isPwm);

public:
  int pin;
  int value;
  int defaultValue = -1;

  GpioPin(int pin, int defaultValue = -1);

  virtual int read();

  virtual void write(int val);

  virtual int autoMode(int val);

  virtual void reset();

  // #TODO Decide wether should be moved to GpioFactory or GpioRemoteService
  std::string dump();
};

/*
 ###################
 ### OUTPUT PINS ###
 ###################
*/

// DOP
class DigitalOutputPin : GpioPin {
public:
  DigitalOutputPin(int pinNb, bool defaultPinState = false);
  void write(bool pinState);
};

// PWM
class GpioPwmPin : GpioPin {
public:
  int freq = 10000;
  int res = 8;
  int channel = 0;

  GpioPwmPin(int pin, int chan, int freq = 10000, int res = 8,
             int defaultValue = -1);

  void write(int value = 0);

  void parse(std::string pinConfStr);

  std::string dump();
};

/*
 ##################
 ### INPUT PINS ###
 ##################
*/

// DIP
class DigitalInputPin : GpioPin {
public:
  DigitalInputPin(int pinNb);
};

// AIP
class AnalogInputPin : GpioPin {
public:
  AnalogInputPin(int pinNb, int defaultVal = 0);
};

/*
* Monitor pin to trigger event when pin state change
* TDIP
*/
class TriggerPin : GpioPin {
public:
  TriggerPin(int pinNb, bool defaultLow = true);
};

/******************
 *** DEPRECATED ***
 ******************/

// enum GPIO_PIN_ACTIONS { READ, WRITE, INIT };

// enum GPIO_PIN_TYPES { ANALOG_PIN, PWM_PIN };

// struct GPIO_PIN_STATE {
//   int val;
//   int freq;
//   int res;
//   int pwmChannel;
//   GPIO_TYPES type;
// };