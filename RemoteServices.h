#include <ArduinoJson.h>
#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <RemoteServiceListener.h>
#include <WebServer.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>
#include <string>

// #define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

using namespace httpsserver;

/*
 * Acting as a proxy or dispatcher for other (sub)services
 * All messages received on main websocket route are rerouted to
 * subservices inheriting the RemoteServiceListener class
 * based on service name provided in following JSON message structure:
 * {
 *    service: targeted subservice
 *    payload: service payload
 * }
 *
 *  Example of remote sub-services
 *  - GPIO control
 *  - FS operations (usecase: read log file, change settings file)
 *  - LOGS: flush logs buffer
 *  - ADMIN setup: ESP settings
 *  - MONITORING:
 */

class RemoteService : public WebsocketHandler {
public:
  // STATIC MEMBERS
  static std::map<std::string, RemoteService *> registeredServices;

  // ws route registration on secured webserver
  //   static WebsocketNode *webSocketNode;
  static std::map<int, RemoteService *>
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

  static void registerDefaultService();

  // METHODS
protected:
  RemoteService(int clientId);

public:
  // Inherited from base class
  // This method is called when a message arrives directly to the service
  // otherwise common callback from factory is used instead
  virtual void onMessage(WebsocketInputStreambuf *input);
  // Handler function on connection close
  void onClose();
};

/****************************
 *** INSTANCE DEFINITIONS ***
 ****************************/

RemoteService::RemoteService(int clientId)
    : WebsocketHandler(), clientId(clientId) {
  LogStore::info("[RemoteService] Client #" + std::to_string(clientId) +
                 " connected");
}

// When the websocket is closing, we remove the client from the array
void RemoteService::onClose() {
  LogStore::info("[RemoteService] Client #" + std::to_string(clientId) +
                 " disconnected");
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

void RemoteService::registerDefaultService() {
  std::string defaultRoute("/ws");
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(defaultRoute, &instanceOnClientConnect);
  LogStore::info("[RemoteService::initDefaultService] on route: " +
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
void RemoteService::registerService(std::string serviceRoute,
                                    WebsocketHandler *(*handlerBuilder)()) {
  // create dedicated socket node
  WebsocketNode *webSocketNode =
      new WebsocketNode(serviceRoute, handlerBuilder);
  LogStore::info("[RemoteService::registerService] on route: " + serviceRoute);
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

/*
 * Default base implementation dispatching message to corresponding service
 * can be overridden by providing custom callback at service registration
 */

void RemoteService::onMessage(WebsocketInputStreambuf *inbuf) {
  // Get the input message
  std::ostringstream ss;
  std::string incomingMsg;
  ss << inbuf;
  incomingMsg = ss.str();
  // this->send("[LogRemoteService::onMessage]", SEND_TYPE_TEXT);
  // readFile(CONFIG_FILE);
  // std::cout << "[GpioRemoteService::unpackMsg] Incoming message " <<
  // incomingMsg
  //           << std::endl;
  // StaticJsonDocument<JSON_MSG_SIZE> root;
  // // convert to a json object
  // DeserializationError error = deserializeJson(root, incomingMsg);

  // // root level props
  // std::string serviceName = root["serviceName"];
  // // find corresponding sub service
  // auto subService = RemoteService::registeredServices.find(serviceName);
  // if (subService != RemoteService::registeredServices.end()) {
  //   // forward message to subservice handler
  //   subService->second->onMessage(inbuf);
  // } else {
  //   LogStore::info("[RemoteService::onMessage] no subservice matching " +
  //                  subService + " was found");
  // }
  std::string outgoingMsg = RemoteServiceListener::dispatchMsg(incomingMsg);
  if (outgoingMsg.length() > 0) {
    this->send(outgoingMsg, SEND_TYPE_TEXT);
  }
}

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<int, RemoteService *> RemoteService::instances;
std::map<std::string, RemoteService *> RemoteService::registeredServices;

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
WebsocketHandler *RemoteService::instanceOnClientConnect() {
  RemoteService *instance = NULL;
  int clientNb = RemoteService::instances.size();
  if (RemoteService::instances.size() < MAX_CLIENTS) {
    instance = new RemoteService(clientNb);
    RemoteService::instances.insert({clientNb, instance});
  } else {
    LogStore::info(
        "[RemoteService::instanceOnClientConnect] reject max number of "
        "client reached #" +
        std::to_string(clientNb));
  }
  // will be cast to WebsocketHandler*
  return instance;
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