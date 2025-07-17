#include <ApiModule.h>
#include <ArduinoJson.h>
#include <GpioPin.h>
#include <LogStore.h>
#include <System.h>
#include <string>
#include <Notifications.h>

#define API_MODULE "test"
/*
{
  type: apiRequest,
  apiReq: {
    module: TestModule,
    submod: testSandbox,
    input: ""
  }
}

{
  type: evtInject,
  event: {
    type: PIN
    data: {

    }
  }
}
*/

// class TestModule : public MessageListener {
class TestModule : public ApiModule, EventHandler {
public:
  static TestModule &instance();

private:
  TestModule();
  std::string testSandbox(std::string msg);

public:
  std::string onApiCall(Packet &msg);
  void onEvent(Event evt);
  void onMsgAck(Packet &msg);

  // void onEvent(std::string eventData);
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/

TestModule &TestModule::instance() {
  static TestModule singleton;
  return singleton;
}

TestModule::TestModule() : EventHandler("PIN"), ApiModule(API_MODULE) {};
// TestModule::TestModule() : EventHandler(EventTypeMap[EventType::TST]) {}

std::string TestModule::onApiCall(Packet &msg) {
  LogStore::info("[TestModule::onCall] ");

  // std::string apiCommand = apiInput["cmd"];
  if (msg.cmd == "sandbox") {
    return testSandbox(msg.data);
  } else if (msg.cmd == "msgACK") {
    onMsgAck(msg);
  }
  return "";
}

void TestModule::onEvent(Event evt){
  LogStore::info("[TestModule::onEvent] ");
};

// void TestModule::onEvent(std::string eventData) {
//   LogStore::info("[TestModule::onEvent] " + eventData);
//   JsonDocument testInput;
//   DeserializationError error = deserializeJson(testInput, eventData);

//   // // extract wrapMsg and send over LORA
//   std::string testModule = testInput["module"];
//   LogStore::dbg("[TestModule::onEvent] testModule" + testModule);

//   // std::string type = msgData["type"];
//   if (testModule == "sandbox") {
//     testSandbox(eventData);
//   }
//   // return "";
// }

std::string TestModule::testSandbox(std::string inputData) {
  LogStore::info("[TestModule::testSandbox] call " + inputData);
  std::string res("");
  // serializeJsonPretty(System::cfgStore.certKeyContent(), res);
  // LogStore::info("[TestModule::testSandbox] dump key file" + res);
  Packet outgoingMsg(LinkInterfaceType::LORA);
  outgoingMsg.api = API_MODULE;
  outgoingMsg.cmd = "sandbox";
  outgoingMsg.data = "";
  outgoingMsg.ack = true;
  std::string outgoingMsgContent = outgoingMsg.serialize();
  LoraInterface::instance().sendText(outgoingMsgContent);
  SndNotif::instance().bips();
  return res;
}

void TestModule::onMsgAck(Packet &msg) {
  SndNotif::instance().bips();
}

#undef API_MODULE