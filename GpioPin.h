#pragma once

#include <cstddef>
#include <iostream>
#include <string>

/*
 *   PIN model, last level before hardware implementing GPIO's
 *.  basic operation: read, write, dump
 */

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
 *  PWM PIN
 */

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