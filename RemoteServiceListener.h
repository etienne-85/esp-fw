#include <defaults.h>
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

/*
 * Each inheriting class will be registered as subservice
 * only singleton instance is allowed per inheriting class (or declared service)
 * when message is received, matching registered instance will be chosen to
 * process incoming message
 */
class RemoteServiceListener {
  static std::map<std::string, RemoteServiceListener *> registeredServices;

protected:
  RemoteServiceListener(std::string serviceName);

public:
  static std::string dispatchMsg(std::string incomingMsg);
  // TO BE IMPLEMENTED IN CHILD CLASS
  virtual std::string processMsg(std::string incomingMsg) = 0;
};

RemoteServiceListener::RemoteServiceListener(std::string serviceName) {
  LogStore::info("[RemoteServiceListener] instancing service " + serviceName);
  RemoteServiceListener::registeredServices.insert({serviceName, this});
}

// STATIC DEF
std::map<std::string, RemoteServiceListener *>
    RemoteServiceListener::registeredServices;

// STATIC
std::string RemoteServiceListener::dispatchMsg(std::string incomingMsg) {
  StaticJsonDocument<JSON_MSG_SIZE> root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);

  // root level props
  std::string serviceName = root["serviceName"];
  LogStore::info(
      "[RemoteServiceListener::dispatchMsg] receive message for service " +
      serviceName);
  // find corresponding sub service
  auto matchingService =
      RemoteServiceListener::registeredServices.find(serviceName);
  if (matchingService != RemoteServiceListener::registeredServices.end()) {
    // forward message to subservice handler
    std::string reply = matchingService->second->processMsg(incomingMsg);
    return reply;

  } else {
    LogStore::info(
        "[RemoteService::onMessage] no registered service matching " +
        serviceName);
  }
  return NULL;
}