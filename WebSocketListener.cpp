#include <WebServer.h>
#include <WebSocketListener.h>
#include <cstddef>
#include <ostream>

/**
 *   WebSocketListener
 **/

// SINGLETON
// template <int SERVER_PORT, const char *SOCKET_PATH>
// WebSocketService<SERVER_PORT, SOCKET_PATH> *
//     WebSocketService<SERVER_PORT, SOCKET_PATH>::instance(NULL);

// template <int SERVER_PORT, const char *SOCKET_PATH>
// std::vector<WebSocketListener *> *
//     WebSocketService<SERVER_PORT, SOCKET_PATH>::wsListeners =
//         new std::vector<WebSocketListener *>();

std::map<std::string, WebSocketListener *> *WebSocketListener::recipients =
    new std::map<std::string, WebSocketListener *>();

/*
 * private constructor:
 * - create new websocket
 * - register websocket to default webserver
 */
WebSocketListener::WebSocketListener(std::string routeKey)
    : webSocket(routeKey.c_str()) {
  std::cout << "[WebSocketListener] create listener for route " << routeKey
            << std::endl;
  WebServer::getDefault().webServer.addHandler(&webSocket);
  webSocket.onEvent(eventHandler);
  (*WebSocketListener::recipients)[routeKey] = this;
  // WebService<SERVER_PORT>::serviceTreePath +=
  //     "\n    |__ [WebSocketService:" + std::to_string(SERVER_PORT) +
  //     SOCKET_PATH + "]";
  // WebServerService::instance(wsKey.first)->webServer.addHandler(&webSocket);
  // instance = this;
  // std::cout << " websocket listening on port "
  //           << wsKey.first << " path " << wsKey.second << std::endl;
}

WebSocketListener *WebSocketListener::instance(std::string routeKey) {
  auto match = WebSocketListener::recipients->find(routeKey);
  // check if service already exists, otherwise create one
  return match != WebSocketListener::recipients->end()
             ? match->second
             : new WebSocketListener(routeKey);
}

WebSocketListener *WebSocketListener::findRecipient(std::string route) {
  auto match = WebSocketListener::recipients->find(route);
  return match != WebSocketListener::recipients->end()
             ? (*WebSocketListener::recipients)[route]
             : NULL;
}

void WebSocketListener::eventHandler(AsyncWebSocket *server,
                                     AsyncWebSocketClient *client,
                                     AwsEventType type, void *arg,
                                     uint8_t *data, size_t len) {
  std::string wsRoute(server->url());
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("[WebSocket] client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("[WebSocket] client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    Serial.println("[WebSocket] Data received");
    dispatchMsg(arg, data, len, client->id(), wsRoute);
    // dispatch(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void WebSocketListener::dispatchMsg(void *arg, uint8_t *data, size_t len,
                                    int clientId, std::string route) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    // convert raw data input
    std::string rawMsg(data, data + len);
    std::cout << "[WebSocketListener] Forwarding message received from client #"
              << clientId << " to registered listener for route " << route
              << std::endl;
    /*** Forward message to all listeners ***/
    // singleton option
    // if (instance != nullptr)
    // instance->processMsg(rawMsg);
    // internal listeners option
    // for (auto wsl : *wsListeners) {
    //   wsl->processMsg(rawMsg);
    // }
    // unique external listener
    auto recipient = WebSocketListener::findRecipient(route);
    if (recipient != NULL) {
      recipient->unpackMsg(rawMsg);
    } else {
      std::cout << "[WebSocketListener] no registered listener for route "
                << route << std::endl;
    }
    // multiple external listeners option
    // for (auto wsl : *WebSocketListener<WS_PORT, WS_PATH>::instances) {
    //   wsl->processMsg(rawMsg);
    // }
    // external async listening option
    // rawMessage = rawMsg;
    // messageCount++;
    /*** Dispatch message to all connected clients ***/
    std::string replyMsg =
        ""; // "Message processed by " + std::to_string(wsListeners->size()) + "
            // wsservices";
    std::cout
        << "[WebSocketListener > dispatch] reply to all connected clients "
        << replyMsg << std::endl;
    // notify server response to all connected clients
    // String dataOut(replyMsg.c_str());
    // aws.textAll(dataOut);
  }
}

std::string
// WebSocketListener<WS_PORT, WS_PATH>::processMsg(std::string rawMsg) {
WebSocketListener::unpackMsg(std::string rawMsg) {
  std::cout << "[WebSocketListener > processMsg]" << rawMsg << std::endl;
  return std::string("msg handler Not Implemented");
}

void WebSocketListener::listen(int wsPort, const char *wsPath) {
  std::cout << "[WebSocketListener] listening on /" << wsPath << ":" << wsPort
            << std::endl;
}

// internal handler to be overriden in children class
// template <int SERVER_PORT, const char *SOCKET_PATH>
// std::string
// WebSocketService<SERVER_PORT, SOCKET_PATH>::processMsg(std::string rawMsg) {
//   std::cout << "[WebSocketService:" << SERVER_PORT << SOCKET_PATH
//             << "] <processMsg> " << rawMsg << std::endl;
//   return std::string("msg handler Not Implemented");
// }

/**
 * WebSocketListener
 **/
/*
template <int WS_PORT, const char *WS_PATH>
int WebSocketListener<WS_PORT, WS_PATH>::messageCount(0);

template <int WS_PORT, const char *WS_PATH>
std::vector<WebSocketListener<WS_PORT, WS_PATH> *> *
    WebSocketListener<WS_PORT, WS_PATH>::instances =
        new std::vector<WebSocketListener<WS_PORT, WS_PATH> *>();

template <int WS_PORT, const char *WS_PATH>
WebSocketListener<WS_PORT, WS_PATH>::WebSocketListener() {
  // WebSocketService<WS_PORT, WS_PATH>::wsListeners->push_back(this);
  instances->push_back(this);
  std::cout << "[WebSocketListener] " << instances->size()
            << " registered listener " << std::endl;
}
template <int WS_PORT, const char *WS_PATH>
void WebSocketListener<WS_PORT, WS_PATH>::asyncListenForwardLoop() {
  if (WebSocketService<WS_PORT, WS_PATH>::messageCount != messageCount) {
    messageCount = WebSocketService<WS_PORT, WS_PATH>::messageCount;
    std::cout << "[WebSocketListener > listenLoop] incoming message #"
              << messageCount << " => forward to " << instances->size()
              << " listeners " << std::endl;
    // notify all listeners
    for (auto l : *instances) {
      // std::cout << "[WebSocketListener] Forward message to " << std::endl;
      // l->processMsg(WebSocketService<WS_PORT, WS_PATH>::rawMessage);
      l->processMsg("toto");
    }
    // reset message counter
    // WebSocketService<WS_PORT, WS_PATH>::messageCount = 0;
    // return std::string("msg handler Not Implemented");
  }
}
*/
// template <int WS_PORT, const char *WS_PATH>

// WebSocketListener::dispatchMsg(std::string msg, std::string dest) {
//   (*WebSocketListener::instances)[dest]->
// }
