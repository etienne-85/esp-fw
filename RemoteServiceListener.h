#pragma once
#include <map>

/*
 * Each inheriting class will be registered as service and must be singleton
 * when message is received, it will be dispatched to matching declared service
 * instance
 * Service can be reached from different interfaces (WS, LORA)
 */
class RemoteServiceListener {
  static std::map<std::string, RemoteServiceListener *> registeredServices;

protected:
  RemoteServiceListener(std::string serviceId);

public:
  static std::string dispatchMsg(std::string incomingMsg, int clientId = -1);
  // TO BE IMPLEMENTED IN CHILD CLASS
  virtual std::string onServiceCall(std::string incomingMsg,
                                    int clientId = -1) = 0;
};