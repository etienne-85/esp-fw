#include <ApiModule.h>
#include <ArduinoJson.h>
#include <Events.h>
#include <LogStore.h>
#include <MessageInterface.h>

std::string MessageInterface::onMessage(std::string incomingMsg) {
  std::string res("");
  JsonDocument originalMsg;
  // convert to a json object
  DeserializationError error = deserializeJson(originalMsg, incomingMsg);
  std::string interfaceType = "<type>"; // LORA or WS
  LogStore::dbg("[MessageInterface::onMessage] received message on" +
                interfaceType + " interface: " + incomingMsg);
  // root msg level props
  std::string msgType = originalMsg["type"]; // need ?
  if (msgType == "apiCall") {
    // TODO msgContext
    // JsonDocument msgContext;
    // msgContext["clientKey"] = clientKey;
    // msgContext["interfaceType"] = interfaceType;
    // msgRoot["context"] = msgContext;
    std::string patchedMsg(incomingMsg);
    res = ApiModule::dispatchApiRequest(patchedMsg);
  } else if (msgType == "evtInject") {
    injectEvent(incomingMsg);
  }
  return res;
}

std::string MessageInterface::injectEvent(std::string incomingMsg) {
  JsonDocument msgRoot;
  // convert to a json object
  DeserializationError error = deserializeJson(msgRoot, incomingMsg);
  std::string interfaceType = "<type>";
  LogStore::dbg("[MessageInterface::onMessage] received message on" +
                interfaceType + " interface: " + incomingMsg);
  // root msg level props
  std::string msgType = msgRoot["type"]; // need ?
  // TODO
  // std::string msgSrc = type; // interface type (LORA,WS)
  // std::string msgCtx = msgRoot["ctx"];
  // std::string clientKey = msgCtx["clientKey"];
  // int msgTime = msgRoot["time"];
  // if (msgTime) {
  //   System::time.sync(msgTime);
  // }
  std::string msgMode = msgRoot["mode"]; // sync or async
  bool bypassEvtQueue(msgMode == "sync");
  // event level props
  JsonObject evt(msgRoot["evt"]);
  std::string evtType = evt["type"];
  std::string evtData = evt["data"];
  EventContext evtCtx; // use default, customise any required fields
  evtCtx.origin = EventOrigin::REMOTE;
  // evtCtx.timestamp = msgTime ? msgTime : evtCtx.time;
  // evtCtx.priority = 0;
  Event event;
  event.type = evtType;
  event.data = evtData;
  event.context = evtCtx;
  EventQueue::pushEvent(event, bypassEvtQueue);
  LogStore::dbg("[MessageInterface::onMessage] " + evtType);
  return "";
};