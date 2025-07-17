#include <Notifications.h>

int SndNotif::pinDefault = DEFAULT_BUZZER_PIN;
int SndNotif::buzDefaultHigh = false;

SndNotif &SndNotif::instance() {
  static SndNotif singleton(SndNotif::pinDefault, SndNotif::buzDefaultHigh);
  return singleton;
}

SndNotif::SndNotif(int pinNb, bool buzzerType) : EventHandler("SND_NOT") {
  buzPin = new DigitalOutputPin(pinNb, true);
};

void SndNotif::onEvent(Event evt){

};

void SndNotif::bips(int count, int duration, int bipsDelay) {
  for (int i = 0; i < count; i++) {
    buzPin->write(!buzPin->defaultValue);
    delay(duration);
    buzPin->write(buzPin->defaultValue);
    delay(bipsDelay);
  }
}