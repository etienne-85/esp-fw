#include <ArduinoJson.h>
#include <CommonObj.h>
#include <LogStore.h>
#include <iostream>
#include <utils.h>
/*
###############
##### MSG #####
###############
*/

Msg::Msg(MessageInterfaceType msgInterfaceType) {
  interface = msgInterfaceType;
}

void Msg::parseContent(std::string msgContent) {
  JsonDocument msgData;
  // convert to a json object
  DeserializationError error = deserializeJson(msgData, msgContent);
  std::string sMsgId = msgData["key"];
  std::string sClientKey = msgData["cli"];
  bool ackDelivery = msgData["ack"];
  std::string apiModuleCall = msgData["cmd"]; // format module:call
  std::string sObjContent = msgData["obj"];
  std::vector<std::string> items = splitFromCharDelim(apiModuleCall, ':');
  apiModule = items.at(0);
  apiCall = items.at(1);
  objContent = sObjContent;
  // content = msgContent;
  msgId = sMsgId;
  clientKey = sClientKey;
  acknowledgeDelivery = ackDelivery;
  // interface = interfaceSrc;
}

std::string Msg::serialize() {
  // convert object to JSON
  JsonDocument msgData;
  msgData["key"] = msgId;
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