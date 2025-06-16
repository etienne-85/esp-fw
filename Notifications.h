#pragma once
#include <GpioPin.h>
#include <Events.h>

#define DEFAULT_BUZZER_PIN 16
#define DEFAULT_LED_PIN 17

class SndNotif : public EventHandler {
  static int pinDefault;

public:
  static SndNotif &instance(int commonPin = DEFAULT_BUZZER_PIN);

private:
  DigitalOutputPin *buzPin;
  SndNotif(int pinId);

public:
  void onEvent(Event evt);
  void bips(int count = 2, int duration = 20, int bipsDelay = 100);
};

class LedNotif : public EventHandler {
public:
  static LedNotif &instance();

private:
  DigitalOutputPin *ledPin;
  LedNotif(int pinId);

public:
  void onEvent(Event evt);
  void blinkTwice();
};