#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <RemoteLogService.h>
#include <RemoteServiceListener.h>
#include <WebServer.h>
#include <WsRemoteInterface.h>
/****************************
 *** INSTANCE DEFINITIONS ***
 ****************************/

WsRemoteInterface::WsRemoteInterface(int clientId)
    : WebsocketHandler(), clientId(clientId) {
  LogStore::info("[WsRemoteInterface] Client #" + std::to_string(clientId) +
                 " connected");
}

// When the websocket is closing, we remove the client from the array
void WsRemoteInterface::onClose() {
  LogStore::info("[WsRemoteInterface] Client #" + std::to_string(clientId) +
                 " disconnected");
  // remove any subscriptions of client
  LogRemoteService::instance().unsubscribe(clientId);
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

void WsRemoteInterface::registerDefaultServiceRoute() {
  std::string defaultRoute("/ws");
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(defaultRoute, &instanceOnClientConnect);
  LogStore::info("[WsRemoteInterface::initDefaultService] on route: " +
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
void WsRemoteInterface::registerService(std::string serviceRoute,
                                        WebsocketHandler *(*handlerBuilder)()) {
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(serviceRoute, handlerBuilder);
  LogStore::info("[WsRemoteInterface::registerService] on route: " +
                 serviceRoute);
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

/*
 * Default base implementation dispatching message to corresponding service
 * can be overridden by providing custom callback at service registration
 */

void WsRemoteInterface::onMessage(WebsocketInputStreambuf *inbuf) {
  // Get the input message
  std::ostringstream ss;
  std::string incomingMsg;
  ss << inbuf;
  incomingMsg = ss.str();
  LogStore::dbg("[RemoteService::onMessage] clientId: " +
                std::to_string(clientId));

  std::string outgoingMsg =
      RemoteServiceListener::dispatchMsg(incomingMsg, clientId);
  if (outgoingMsg.length() > 0) {
    // LogStore::info("[RemoteService::onMessage] sending reply: " +
    // outgoingMsg);
    LogStore::info("[RemoteService::onMessage] sending REPLY ");
    this->send(outgoingMsg, SEND_TYPE_TEXT);
  } else {
    // LogStore::info("[WsRemoteInterface::onMessage] empty message => no reply
    // sent");
  }
}

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<int, WsRemoteInterface *> WsRemoteInterface::instances;
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
WebsocketHandler *WsRemoteInterface::instanceOnClientConnect() {
  WsRemoteInterface *instance = NULL;
  int clientNb = WsRemoteInterface::instances.size();
  if (WsRemoteInterface::instances.size() < MAX_CLIENTS) {
    instance = new WsRemoteInterface(clientNb);
    WsRemoteInterface::instances.insert({clientNb, instance});
  } else {
    LogStore::info(
        "[WsRemoteInterface::instanceOnClientConnect] reject max number of "
        "client reached #" +
        std::to_string(clientNb));
  }
  // will be cast to WebsocketHandler*
  return instance;
}

void WsRemoteInterface::notifyClient(int clientKey, std::string notification) {
  WsRemoteInterface *clientInstance = WsRemoteInterface::instances[clientKey];
  clientInstance->send(notification, SEND_TYPE_TEXT);
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