#include <RemoteServiceListener.h>
#include <LogStore.h>
#include <ArduinoJson.h>

RemoteServiceListener::RemoteServiceListener(std::string serviceId) {
  LogStore::info("[RemoteServiceListener] instancing service " + serviceId);
  RemoteServiceListener::registeredServices.insert({serviceId, this});
}

// STATIC DEF
std::map<std::string, RemoteServiceListener *>
    RemoteServiceListener::registeredServices;

// STATIC
std::string RemoteServiceListener::dispatchMsg(std::string incomingMsg,
                                               int clientId) {
  JsonDocument inputRoot;
  // convert to a json object
  DeserializationError error = deserializeJson(inputRoot, incomingMsg);

  // root level props common to all services
  std::string serviceId = inputRoot["svcId"];
  int timestamp = inputRoot["timestamp"];
  // find corresponding sub service
  auto matchingService =
      RemoteServiceListener::registeredServices.find(serviceId);
  if (matchingService != RemoteServiceListener::registeredServices.end()) {
    // LogStore::info("[RemoteServiceListener::dispatchMsg] dispatch to service
    // " +serviceId); forward message to subservice handler
    std::string dataOut =
        matchingService->second->onServiceCall(incomingMsg, clientId);
    // default empty reply
    std::string outgoingMsg("");
    bool expectedReply = timestamp > 0 || dataOut.length() > 0;
    // fill with data from service if any
    if (expectedReply) {
      JsonDocument replyData;
      deserializeJson(replyData, dataOut);
      // Build the JSON reply including service output
      // and additional fields (serviceId, timestamp)
      JsonDocument replyRoot;
      replyRoot["svcId"] = serviceId;
      if (timestamp > 0) {
        LogStore::info(
            "[RemoteServiceListener::extractMsg] timestamp provided " +
            std::to_string(timestamp) + " => expected reply confirmation");
        replyRoot["timestamp"] = timestamp;
      }
      if (dataOut.length() > 0) {
        replyRoot["svcOut"] = replyData;
      }
      serializeJsonPretty(replyRoot, outgoingMsg);
    }
    return outgoingMsg;
  } else {
    LogStore::info(
        "[RemoteServiceListener::dispatchMsg] no registered service matching " +
        serviceId);
  }
  return "";
}