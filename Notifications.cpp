#include <Notifications.h>

int SndNotif::pinDefault = DEFAULT_BUZZER_PIN;

SndNotif &SndNotif::instance(int defaultPin) {
  if (defaultPin) {
    SndNotif::pinDefault = defaultPin;
  }
  static SndNotif singleton(SndNotif::pinDefault);
  return singleton;
}

SndNotif::SndNotif(int pinNb) : EventHandler("SND_NOT") {
  buzPin = new DigitalOutputPin(pinNb, false);
};

void SndNotif::onEvent(Event evt){

};

void SndNotif::bips(int count, int duration, int bipsDelay) {
  for (int i = 0; i < count; i++) {
    buzPin->write(true);
    delay(duration);
    buzPin->write(false);
    delay(bipsDelay);
  }
}