#include <ArduinoJson.h>
#include <CommonObj.h>
#include <LogStore.h>
#include <System.h>
#include <iostream>
#include <utils.h>

/*
###############
##### MSG #####
###############
*/

Msg::Msg(MessageInterfaceType msgInterfaceType): interface(msgInterfaceType) {
  // by default device local elapsed time
  // int deviceLocalTime = millis();
  // msgId = std::to_string(deviceLocalTime);
}

void Msg::parseContent(std::string msgContent) {
  JsonDocument msgData;
  // convert to a json object
  DeserializationError error = deserializeJson(msgData, msgContent);
  std::string id = msgData["id"];
  std::string src = msgData["src"];
  std::string dst = msgData["dst"];
  bool ack = msgData["ack"];
  std::string apiModuleCall = msgData["cmd"]; // format apiModule:call
  std::string obj = msgData["obj"];
  std::vector<std::string> items = splitFromCharDelim(apiModuleCall, ':');
  apiModule = items.at(0);
  apiCall = items.at(1);
  objContent = obj;
  // content = msgContent;
  msgId = id;
  sender = src;
  target = dst;
  acknowledgeDelivery = ack;
  // interface = interfaceSrc;
}

std::string Msg::serialize() {
  // convert object to JSON
  JsonDocument msgData;
  if (msgId.length() > 0) {
    msgData["id"] = msgId;
  }
  if (target.length() > 0) {
    std::string deviceId = System::cfgStore.configContent()["deviceId"];
    msgData["src"] = deviceId;
    msgData["dst"] = target;
  }
  if (acknowledgeDelivery) {
    msgData["ack"] = true;
  }
  msgData["cmd"] = apiModule + ":" + apiCall;
  msgData["obj"] = objContent;
  std::string msgContent("");
  serializeJson(msgData, msgContent);
  return msgContent;
}

// /*
// Transport API call over MSG
// */
// std::string ApiCall::toMsg() {
//   JsonDocument apiCall;
//   apiCall["module"] = apiModule;
//   apiCall["call"] = call;
//   apiCall["data"] = data;
//   std::string apiCallMsg;
//   serializeJson(req, apiCallMsg);
//   return apiCallMsg;
// }

ForeignEvent::ForeignEvent() {}

ForeignEvent::ForeignEvent(Event evt, std::string source) : Event(evt) {
  context.source = source;
}

// create event object from JSON
void ForeignEvent::fromObjContent(std::string objDataMsg) {
  JsonDocument objData;
  // convert content to a json object
  DeserializationError error = deserializeJson(objData, objDataMsg);
  // parse JSON and fill object fields
  std::string evtType = objData["type"];
  std::string evtContent = objData["content"];
  std::string evtSrc = objData["evtSender"];
  EventContext evtContext; // use default, customise any required fields
  evtContext.source = evtSrc;
  // evtCtx.timestamp = msgTime ? msgTime : evtCtx.time;
  // evtCtx.priority = 0;
  type = evtType;
  content = evtContent;
  context = evtContext;
}

// convert event object to JSON
std::string Event::toObjContent() {
  JsonDocument jsonObj;
  jsonObj["type"] = type;
  jsonObj["content"] = content;
  jsonObj["time"] = context.timestamp;
  jsonObj["sender"] = context.source;
  std::string jsonMsg("");
  serializeJson(jsonObj, jsonMsg);
  return jsonMsg;
}