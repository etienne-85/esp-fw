#include <ArduinoJson.h>
#include <CommonObj.h>
#include <LogStore.h>
#include <System.h>
#include <utils.h>

/*
###############
##### MSG #####
###############
*/

Packet::Packet(LinkInterfaceType msgInterfaceType): interface(msgInterfaceType) {
  // by default device local elapsed time
  // int deviceLocalTime = millis();
  // msgId = std::to_string(deviceLocalTime);
}

bool Packet::parse(const std::string rawData) {
    JsonDocument parsed;
    
    // Check JSON deserialization
    DeserializationError error = deserializeJson(parsed, rawData);
    if (error) {
        // Log error if needed: Serial.println("JSON parsing failed");
        LogStore::dbg("[Packet::parse] invalid JSON " );
        return false;
    }

    // Extract all fields (ArduinoJson returns defaults for missing fields)
    timestamp = parsed["timestamp"].as<std::string>();
    sender = parsed["src"].as<std::string>();
    target = parsed["dst"].as<std::string>();
    ack = parsed["ack"].as<bool>();
    data = parsed["data"].as<std::string>();
    
    std::string endpoint = parsed["endpoint"].as<std::string>();
    
    // Validate based on string content
    if (timestamp.empty() || endpoint.empty()) {
        return false;
    }
    
    std::vector<std::string> split = splitFromCharDelim(endpoint, ':');
    
    // Check split result and validate components
    if (split.size() < 2 || split[0].empty() || split[1].empty()) {
        return false;
    }
    
    api = split[0];
    cmd = split[1];
    
    return true;  // Parsing successful
}

std::string Packet::serialize() {
  // convert object to JSON
  JsonDocument packetData;
  if (timestamp.length() > 0) {
    packetData["id"] = timestamp;
  }
  if (target.length() > 0) {
    std::string deviceId = System::cfgStore.configContent()["deviceId"];
    packetData["src"] = deviceId;
    packetData["dst"] = target;
  }
  if (ack) {
    packetData["ack"] = true;
  }
  packetData["endpoint"] = api + ":" + cmd;
  packetData["data"] = data;
  std::string rawPacketData("");
  serializeJson(packetData, rawPacketData);
  return rawPacketData;
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