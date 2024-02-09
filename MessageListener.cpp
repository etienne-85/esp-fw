#include <ArduinoJson.h>
#include <LogStore.h>
#include <MessageListener.h>
#include <System.h>

MessageListener::MessageListener(std::string msgType) {
  LogStore::info("[MessageListener::constructor] handle for " + msgType +
                 " message type");
  MessageListener::registeredServices.insert({msgType, this});
}

// STATIC DEF
std::map<std::string, MessageListener *> MessageListener::registeredServices;

// STATIC
std::string MessageListener::dispatchMsg(std::string incomingMsg,
                                         std::string clientKey) {
  JsonDocument msgRoot;
  // convert to a json object
  DeserializationError error = deserializeJson(msgRoot, incomingMsg);

  // root level props common to all services
  std::string handle = msgRoot["handle"];
  int timestamp = msgRoot["timestamp"];
  if (timestamp) {
    System::time.sync(timestamp);
  }
  // find corresponding sub service
  auto msgHandler = MessageListener::registeredServices.find(handle);
  if (msgHandler != MessageListener::registeredServices.end()) {
    // LogStore::info("[MessageListener::dispatchMsg] dispatch to message
    // handler " +msgType); forward message to subservice handler
    std::string dataOut = msgHandler->second->onCall(incomingMsg, clientKey);
    // default empty reply
    std::string outgoingMsg("");
    bool expectedReply = timestamp > 0 || dataOut.length() > 0;
    // fill with data from service if any
    if (expectedReply) {
      JsonDocument replyData;
      deserializeJson(replyData, dataOut);
      // Build the JSON reply including service output
      // and additional fields (msgType, timestamp)
      JsonDocument replyRoot;
      replyRoot["handle"] = handle;
      if (timestamp > 0) {
        LogStore::info("[MessageListener::extractMsg] timestamp provided " +
                       std::to_string(timestamp) +
                       " => expected reply confirmation");
        replyRoot["timestamp"] = timestamp;
      }
      if (dataOut.length() > 0) {
        replyRoot["data"] = replyData;
      }
      serializeJsonPretty(replyRoot, outgoingMsg);
    }
    return outgoingMsg;
  } else {
    LogStore::info("[MessageListener::dispatchMsg] no registered message "
                   "handler matching " +
                   handle);
  }
  return "";
}