#include <ArduinoJson.h>

/*
 * Each inheriting class will be registered as subservice
 * only singleton instance is allowed per inheriting class (or declared service)
 * when message is received, matching registered instance will be chosen to
 * process incoming message
 */
class RemoteServiceListener {
  static std::map<std::string, RemoteServiceListener *> registeredServices;

protected:
  RemoteServiceListener(std::string serviceId);

public:
  static std::string dispatchMsg(std::string incomingMsg);
  // TO BE IMPLEMENTED IN CHILD CLASS
  virtual std::string processMsg(std::string incomingMsg) = 0;
};

RemoteServiceListener::RemoteServiceListener(std::string serviceId) {
  LogStore::info("[RemoteServiceListener] instancing service " + serviceId);
  RemoteServiceListener::registeredServices.insert({serviceId, this});
}

// STATIC DEF
std::map<std::string, RemoteServiceListener *>
    RemoteServiceListener::registeredServices;

// STATIC
std::string RemoteServiceListener::dispatchMsg(std::string incomingMsg) {
  JsonDocument inputRoot;
  // convert to a json object
  DeserializationError error = deserializeJson(inputRoot, incomingMsg);

  // root level props
  std::string serviceId = inputRoot["serviceId"];
  // find corresponding sub service
  auto matchingService =
      RemoteServiceListener::registeredServices.find(serviceId);
  if (matchingService != RemoteServiceListener::registeredServices.end()) {
    LogStore::info(
        "[RemoteService::dispatchMsg] forwarding message to service " +
        serviceId);
    // forward message to subservice handler
    std::string outputData = matchingService->second->processMsg(incomingMsg);

    JsonDocument outputJson;
    deserializeJson(outputJson, outputData);
    // Build the JSON reply including serviceId and service output
    JsonDocument outputRoot;
    outputRoot["serviceId"] = serviceId;
    outputRoot["output"] = outputJson;
    std::string outgoingMsg("");
    serializeJsonPretty(outputRoot, outgoingMsg);
    return outgoingMsg;
  } else {
    LogStore::info(
        "[RemoteService::dispatchMsg] no registered service matching " +
        serviceId);
  }
  return "";
}