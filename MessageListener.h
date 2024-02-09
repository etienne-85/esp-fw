#pragma once
#include <map>

/*
 * Each inheriting class will be registered as service and must be singleton
 * When message is received, it will be dispatched to matching declared service
 * instance
 * Services are exposed to the outside through interfaces (WS, LORA)
 * Services handle and respond to clients' requests received on interface
 * In case of response, services are not aware of
 * - interface types: WS/LORA
 * - client id
 * which are handled at interface level
 * When notifying client of an event, service must target subscriber
 which is identified depends on interface type
 * - LORA: deviceId
 * - WS: opened socket (clientKey)
 *
 */
class MessageListener {
  static std::map<std::string, MessageListener *> registeredServices;
  // static std::map<std::string, MessageListener *> serviceHandlers;
  // static std::map<int, MessageListener *> clientServiceInstances;

protected:
  MessageListener(std::string serviceId);

public:
  static std::string dispatchMsg(std::string incomingMsg,
                                 std::string clientKey = "");
  // TO BE IMPLEMENTED IN CHILD CLASS

  virtual std::string onCall(std::string rawMsg,
                             std::string clientKey = "") = 0;
};