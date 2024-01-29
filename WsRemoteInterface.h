#pragma once
#include <ArduinoJson.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>
#include <string>

using namespace httpsserver;

/*
 * Acting as a proxy or dispatcher for other (sub)services
 * All messages received on main websocket route are rerouted to
 * subservices inheriting the RemoteServiceListener class
 * based on service name provided in following JSON message structure:
 * {
 *    serviceId: targeted subservice
 *    dataIn: service data input
 * }
 *
 * Optional service reply will be formed
 *
 *  {
 *    serviceId: targeted subservice
 *    dataOut: service data output
 * }
 *
 *  Example of remote sub-services
 *  - GPIO control
 *  - FS operations (usecase: read log file, change settings file)
 *  - LOGS: flush logs buffer
 *  - ADMIN setup: ESP settings
 *  - MONITORING:
 */

class WsRemoteInterface : public WebsocketHandler {
public:
  // STATIC MEMBERS
  // static std::map<std::string, RemoteService *> registeredServices;

  // ws route registration on secured webserver
  //   static WebsocketNode *webSocketNode;
  static std::map<int, WsRemoteInterface *>
      instances; // dedicated instance for each connected client

  // each connected client creates a dedicated instance of this class
  static WebsocketHandler *instanceOnClientConnect();

  // MEMBERS
  std::string serviceRoute;
  int clientId = 0;

  // STATIC METHODS
  // public:
  //   static void *registerSubService(std::string subServiceRoute,
  //                                       RemoteService *serviceSingleton);
  static void registerService(std::string serviceRoute,
                              WebsocketHandler *(*handlerBuilder)());

  static void registerDefaultServiceRoute();

  static void notifyClient(int clientKey, std::string notification);

  // METHODS
protected:
  WsRemoteInterface(int clientId);

public:
  // Inherited from base class
  // This method is called when a message arrives directly to the service
  // otherwise common callback from factory is used instead
  virtual void onMessage(WebsocketInputStreambuf *input);
  // Handler function on connection close
  void onClose();
  void sendText(std::string text);
};