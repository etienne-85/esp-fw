#pragma once
#include <ArduinoJson.h>
#include <CommonObj.h>
#include <Events.h>
#include <MessageInterface.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>
#include <string>
using namespace httpsserver;

/*
  Websocket message interface suitable for real time communication
  between ESP and remote clients (mainly browsers)
  Any received message will be dispatched to message handlers
  depending on message handle type
  For sake of simplification, "/ws" is used as default route
  Any remote client connected will create new instance of this class
 */

class WsInterface : public MessageInterface, public WebsocketHandler {
public:
  // STATIC MEMBERS
  // static std::map<std::string, RemoteService *> registeredServices;

  // ws route registration on secured webserver
  //   static WebsocketNode *webSocketNode;
  // dedicated instance for each connected client
  static std::map<std::string, WsInterface *> instances;

  // each connected client creates a dedicated instance of this class
  static WebsocketHandler *instanceOnClientConnect();

  // MEMBERS
  std::string serviceRoute;
  std::string clientKey;

  // STATIC METHODS
  // public:
  //   static void *registerSubService(std::string subServiceRoute,
  //                                       RemoteService *serviceSingleton);
  static void registerService(std::string serviceRoute,
                              WebsocketHandler *(*handlerBuilder)());

  static void registerDefaultServiceRoute();

  static WsInterface *clientInstance(std::string clientKey);
  static void notifyAll(std::string notification);
  void notifyClient(std::string notification);

  // METHODS
protected:
  WsInterface(std::string clientKey);

public:
  // Inherited from base class
  // This method is called when a message arrives directly to the service
  // otherwise common callback from factory is used instead
  virtual void onMessage(WebsocketInputStreambuf *input);
  // Handler function on connection close
  void onClose();
  void sendText(std::string text);
};