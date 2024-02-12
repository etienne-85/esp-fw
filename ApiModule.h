#pragma once
#include <map>

/*
 * Each inheriting class will be registered as service and must be singleton
 * When api call or request is received, it will be dispatched to matching
 declared service instance
 * Services API is exposed through remote interfaces (WS, LORA)
 * Services handle and respond to API requests received from clients
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
// class ApiRequestHandler {
class ApiModule {
  static std::map<std::string, ApiModule *> registeredApiModules;
  // static std::map<std::string, ApiModule *> serviceHandlers;
  // static std::map<int, ApiModule *> clientServiceInstances;

protected:
  ApiModule(std::string serviceId);

public:
  static std::string dispatchApiRequest(std::string apiRequestMsg);
  // TO BE IMPLEMENTED IN CHILD CLASS

  virtual std::string onApiCall(std::string rawMsg) = 0;
};