#define API_MODULE "Alarm"
#include <CommonObj.h>
#include <GpioPin.h>
#include <LogStore.h>
#include <Notifications.h>

#define MAX_DELAY 10000
#define TRIGGERS_MIN_DELAY 1000
#define ACK_DELAY 10000

/*
 watching PIR_PIN state and triggering specific ALARM event when state change
 ## SIMPLIFICATION ##
- on trigger device: catch ALARM event and send Alarm API call over LORA
- on listener device: specific API call handler listening for alarm trigger
 ## ORIGINAL ##
 enable event forwarding for ALARM event over LORA to notify other LORA clients
  listens to foreign events coming from RemoteAlarmSystem
*/
class AlarmSystem : public EventHandler, ApiModule {
  DigitalOutputPin *buzPin;
  std::vector<int> discardedAlerts;
  int lastTrigger = 0;
  int lastRequest = 0;
  bool receivedAck = false;

public:
  static AlarmSystem &instance(int pinPIR, int pinBUZ);
  AlarmSystem(int pinPIR, int pinBUZ);
  void onEvent(Event evt);
  void onMotionDetected();
  std::string onApiCall(Packet &msg);
  void onMsgAck(Packet &msg);
  void onSoundAlert(Packet &msg);
};

AlarmSystem &AlarmSystem::instance(int pinPIR, int pinBUZ) {
  static AlarmSystem singleton(pinPIR, pinBUZ);
  return singleton;
}

AlarmSystem::AlarmSystem(int pinPIR, int pinBUZ)
    : EventHandler("PIN_TRIGGER"), ApiModule(API_MODULE) {
  LogStore::info("[AlarmSystem::construct] watching PIR state on pin " +
                 std::to_string(pinPIR));
  TriggerPin pinObs(pinPIR);
  // buzPin = new DigitalOutputPin(pinBUZ, false);
}

/*
 *   EVENTS
 */
void AlarmSystem::onEvent(Event evt) { onMotionDetected(); }

// handled on trigger device
void AlarmSystem::onMotionDetected() {
  int current = millis();
  // notify remote device of alarm
  if (current - lastTrigger > TRIGGERS_MIN_DELAY) {
    Packet outgoingMsg(LinkInterfaceType::LORA);
    outgoingMsg.api = API_MODULE;
    outgoingMsg.cmd = "onAlert";
    outgoingMsg.data = "";
    outgoingMsg.ack = true;

    if (current - lastRequest > ACK_DELAY) {
      LogStore::info("[AlarmSystem::onMotionDetected] send alarm trigger #" +
                     outgoingMsg.timestamp);
      lastRequest = current;
      receivedAck = false;
      SndNotif::instance().bips(1, 16, 500);
      std::string outgoingMsgContent = outgoingMsg.serialize();
      LoraInterface::instance().sendText(outgoingMsgContent);
    } else {
      LogStore::info(
          "[AlarmSystem::onMotionDetected] pending request awaiting ACK");
      // SndNotif::instance().bips(2, 5, 100);
    }
  }
  lastTrigger = current;
}

/*
 *   API CALLS
 */
std::string AlarmSystem::onApiCall(Packet &msg) {
  // int current = millis();
  // if (current - lastTrigger < MAX_DELAY)
  if (msg.cmd == "onAlert") {
    onSoundAlert(msg);
  } else if (msg.cmd == "msgACK") {
    onMsgAck(msg);
  }
  return "";
}

// API call handled on trigger device
void AlarmSystem::onMsgAck(Packet &msg) {
  LogStore::info("[AlarmSystem::onMsgAck] received ACK #" + msg.timestamp);
  receivedAck = true;
  SndNotif::instance().bips(2, 100, 200);
}

// API call handled on listening device
void AlarmSystem::onSoundAlert(Packet &msg) {
  LogStore::info("[AlarmSystem::onSoundAlert] received alarm trigger #" +
                 msg.timestamp);
  // for (int i = 0; i < 3; i++) {
  //   buzPin->write(true);
  //   delay(20);
  //   buzPin->write(false);
  //   delay(100);
  // }
}
