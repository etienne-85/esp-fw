#include <WebServer.h>
#include <WebSocketListener.h>
#include <cstddef>
#include <ostream>

/**
 *   WebSocketListener
 **/

std::map<std::string, WebSocketListener *> *WebSocketListener::recipients =
    new std::map<std::string, WebSocketListener *>();

/*
 * private constructor:
 * - create new websocket for given path route
 * - register websocket to default webserver
 */
WebSocketListener::WebSocketListener(std::string routeKey)
    : webSocket(routeKey.c_str()) {
  std::cout << "[WebSocketListener] create listener for route " << routeKey
            << std::endl;
  WebServer::instance().webServer.addHandler(&webSocket);
  webSocket.onEvent(eventHandler);
  (*WebSocketListener::recipients)[routeKey] = this;
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
    std::cout << "[WebSocketListener:" << wsRoute << "] Client #"
              << client->id() << " connected from "
              << client->remoteIP().toString().c_str() << std::endl;
    break;
  case WS_EVT_DISCONNECT:
    std::cout << "[WebSocketListener:" << wsRoute << "] Client #"
              << client->id() << " discconnected" << std::endl;
    break;
  case WS_EVT_DATA:
    std::cout << std::endl;
    std::cout << "[WebSocketListener:" << wsRoute << "] *** RECEIVED MSG ***"
              << std::endl;
    dispatchMsg(arg, data, len, client->id(), wsRoute);
    std::cout << "[WebSocketListener:" << wsRoute
              << "] *** END PROCESSING MSG ***" << std::endl;
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

    auto recipient = WebSocketListener::findRecipient(route);
    if (recipient != NULL) {
      recipient->unpackMsg(rawMsg);
    } else {
      std::cout << "[WebSocketListener::dispatch:" << route
                << "] no registered listener for route " << route << std::endl;
    }
  }
}

/*
 * To be implemented in child service
 */
void WebSocketListener::unpackMsg(std::string rawMsg) {
  std::cout << "[WebSocketListener > processMsg]" << rawMsg << std::endl;
}

void WebSocketListener::loop() {
  // send alive signal to all clients listening on current route
}

// notify server response to all clients
// WebSocketListener::reply(std::string replyMsg){
//     String dataOut(replyMsg.c_str());
//     aws.textAll(dataOut);
// }

// void WebSocketListener::listen(int wsPort, const char *wsPath) {
//   std::cout << "[WebSocketListener] listening on /" << wsPath << ":" <<
//   wsPort
//             << std::endl;
// }