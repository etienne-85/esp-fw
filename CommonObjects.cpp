#include <ArduinoJson.h>
#include <CommonObjects.h>
#include <LogStore.h>
#include <iostream>
#include <utils.h>

// ApiCall::ApiCall(std::string apiModule, std::string call, std::string data)
//     : apiModule(apiModule), call(call), data(data){};

/*
Parse API call from MSG
*/
void ApiCall::fromMsg(std::string msg) {
  JsonDocument msgData;
  // convert to a json object
  DeserializationError error = deserializeJson(msgData, msg);
  std::string apiModuleCall = msgData["apiCall"];
  std::string sData = msgData["data"];
  std::vector<std::string> items = splitFromCharDelim(apiModuleCall, ':');
  apiModule = items.at(0);
  call = items.at(1);
  data = sData;
}

std::string ApiCall::toMsg() {
  // convert object to JSON
  JsonDocument jsonObj;
  jsonObj["apiCall"] = apiModule + ":" + call;
  jsonObj["data"] = data;
  std::cout << std::hex << apiModule << std::endl;
  std::string jsonMsg("");
  serializeJson(jsonObj, jsonMsg);
  return jsonMsg;
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
void ForeignEvent::fromObjContent(std::string objContent) {
  JsonDocument objData;
  // convert content to a json object
  DeserializationError error = deserializeJson(objData, objContent);
  // parse JSON and fill object fields
  std::string evtType = objData["evtType"];
  std::string evtData = objData["evtData"];
  std::string evtSrc = objData["evtSender"];
  EventContext evtContext; // use default, customise any required fields
  evtContext.source = evtSrc;
  // evtCtx.timestamp = msgTime ? msgTime : evtCtx.time;
  // evtCtx.priority = 0;
  type = evtType;
  data = evtData;
  context = evtContext;
}

// convert event object to JSON
std::string Event::toObjContent() {
  JsonDocument jsonObj;
  jsonObj["evtType"] = type;
  jsonObj["evtData"] = data;
  jsonObj["evtSender"] = context.source;
  std::string jsonMsg("");
  serializeJson(jsonObj, jsonMsg);
  return jsonMsg;
}