#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <WebServer.h>
#include <WsInterface.h>
/****************************
 *** INSTANCE DEFINITIONS ***
 ****************************/

WsInterface::WsInterface(std::string clientKey)
    : WebsocketHandler(), clientKey(clientKey) {
  LogStore::info("[WsInterface] Client " + clientKey + " connected");
}

// When the websocket is closing, we remove the client from the array
void WsInterface::onClose() {
  LogStore::info("[WsInterface] Client " + clientKey + " disconnected");
  // remove any subscriptions of client
  // LogRemoteService::instance().unsubscribe(clientId);
  // for(int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] == this) {
  //     activeClients[i] = nullptr;
  //   }
  // }
}

/*
 * virtual service handler
 */
// void RemoteService::registerVirtualService(std::string virtualServiceRoute,
//                                            RemoteService *serviceSingleton) {
//   LogStore::info("[RemoteService::registerVirtualService] on virtual route: "
//   +
//                  virtualServiceRoute);
//   RemoteService::registeredServices.insert(
//       {virtualServiceRoute, serviceSingleton});
// }

void WsInterface::registerDefaultServiceRoute() {
  std::string defaultRoute("/ws");
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(defaultRoute, &instanceOnClientConnect);
  LogStore::info("[WsInterface::initDefaultService] WS interface "
                 "available on default route: " +
                 defaultRoute);
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

/*
 * automatically handle service creation when client connects on specific
 * route and directly send message to service.
 * handlerBuilder is a static method from child class able to create service
 * instance
 */
void WsInterface::registerService(std::string serviceRoute,
                                  WebsocketHandler *(*handlerBuilder)()) {
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(serviceRoute, handlerBuilder);
  LogStore::info("[WsInterface::registerService] on route: " + serviceRoute);
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

/*
 * Default base implementation dispatching message to corresponding service
 * can be overridden by providing custom callback at service registration
 */

void WsInterface::onMessage(WebsocketInputStreambuf *inbuf) {
  // Get the input message
  std::ostringstream ss;
  std::string incomingMsg;
  ss << inbuf;
  incomingMsg = ss.str();
  LogStore::dbg("[WsInterface::onMessage] clientKey: " + clientKey);

  // TODO publish MSG event
  std::string outgoingMsg = MessageInterface::onMessage(incomingMsg);
  if (outgoingMsg.length() > 0) {
    // LogStore::info("[RemoteService::onMessage] sending reply: " +
    // outgoingMsg);
    LogStore::info("[WsInterface::onMessage] REPLYING ");
    this->send(outgoingMsg, SEND_TYPE_TEXT);
  } else {
    // LogStore::info("[WsInterface::onMessage] empty message => no reply
    // sent");
  }
}

void WsInterface::notifyClient(std::string notification) {
  WsInterface *theClientInstance = clientInstance(clientKey);
  if (theClientInstance != nullptr) {
    theClientInstance->send(notification, SEND_TYPE_TEXT);
  } else {
    LogStore::dbg("[WsInterface::notifyClient] no client #" + clientKey +
                  " found ");
  }
}

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<std::string, WsInterface *> WsInterface::instances;
// std::map<std::string, RemoteService *> RemoteService::registeredServices;

// WebsocketNode *RemoteService::webSocketNode =
//     new WebsocketNode("/services", &RemoteService::registerClient);

/**
 * Per client service constructor.
 * An instance is created and associated to each client when it first connects
 * on websocket route If having 2 different services on their respective routes
 * `route1`, and `route2` First client connects on `route1` =>
 * service#1instance#1 First client connects on `route2` => service#2instance#1
 * Second client connects on `route1`=> service#1instance#2
 * Second client connects on `route2`=> service#2instance#2
 *
 * 2 clients connected on 2 different routes => 4 total instances
 *
 */
WebsocketHandler *WsInterface::instanceOnClientConnect() {
  WsInterface *instance = NULL;
  int clientNb = WsInterface::instances.size();
  std::string clientKey = "wsClient#" + std::to_string(clientNb);
  if (WsInterface::instances.size() < MAX_CLIENTS) {
    instance = new WsInterface(clientKey);
    WsInterface::instances.insert({clientKey, instance});
  } else {
    LogStore::info(
        "[WsInterface::instanceOnClientConnect] reject max number of "
        "client reached " +
        clientKey);
  }
  // will be cast to WebsocketHandler*
  return instance;
}

WsInterface *WsInterface::clientInstance(std::string clientKey) {
  auto pairItem = WsInterface::instances.find(clientKey);
  return pairItem != WsInterface::instances.end() ? pairItem->second : nullptr;
}

/**
 * Admin service to change esp settings remotely
 */

// void dumpConfig();
// void updateSettings(); // update settings temporarily
// void saveSettings();   // making changes permanent

// class StandaloneRemoteService : RemoteService {};

// class VirtualRemoteService {
//   void extractMessage(std::string incomingMsg);
// };