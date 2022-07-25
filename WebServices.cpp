#include <WebServices.h>
#include <cstddef>
#include <ostream>
#include <string>

/**
 * Service
 */

std::vector<Service *> *Service::instances = new std::vector<Service *>();

Service::Service() {
  instances->push_back(this);
  // std::cout << "[Service] new service " << this->getServiceName() << " #"
  //           << instances->size() << std::endl;
  // printLog(" #" + std::to_string(instances->size()));
}

void Service::printLog(std::string log) {
  std::cout << "[" << this->getServiceName() << "]" << log << std::endl;
}

// static version with no args
void Service::printServiceTree(std::string dummyArg) {
  std::cout << "[Services Tree]" << std::endl;
  for (auto i : *instances) {
    i->printServiceTree(1);
  }
}

// instance version called for each child service
void Service::printServiceTree(int depth) {
  // compute offset from depth
  std::cout << serviceTreePath << std::endl;
  // std::cout << "[Services]"<< instances->size() << std::endl;
  // for (auto i : *instances) {
  //   i->printServiceTree(depth + 1);
  // }
}

/**
 *   WebService
 **/

template <int SERVER_PORT>
AsyncWebServer WebService<SERVER_PORT>::server(SERVER_PORT);

// template <int SERVER_PORT>
// std::vector<WebService<SERVER_PORT> *> *WebService<SERVER_PORT>::instances =
//     new std::vector<WebService<SERVER_PORT> *>();

template <int SERVER_PORT> bool WebService<SERVER_PORT>::serverState = false;

template <int SERVER_PORT> WebService<SERVER_PORT>::WebService() : Service() {
  serviceTreePath += "|__ [WebService:" + std::to_string(SERVER_PORT) + "]";
  // instances->push_back(this);
  // index = instances->size() - 1;
  // std::cout << "[WebService:" << SERVER_PORT << "]"
  //           << " new service #" << index << std::endl;
}

template <int SERVER_PORT> void WebService<SERVER_PORT>::start() {
  if (!WebService<SERVER_PORT>::serverState) {
    std::cout << "[WebService:" << SERVER_PORT
              << "] First instance : starting Server" << std::endl;
    // WebService<SERVER_PORT>::server = new AsyncWebServer(SERVER_PORT);
    WebService<SERVER_PORT>::server.begin();
    WebService<SERVER_PORT>::serverState = true;
    std::cout << "[WebService:" << SERVER_PORT << "] Server running on port "
              << SERVER_PORT << std::endl;
  }
}

/**
 *   WebSocketService
 **/

template <int SERVER_PORT, const char *SOCKET_PATH>
AsyncWebSocket WebSocketService<SERVER_PORT, SOCKET_PATH>::aws("/ws");

template <int SERVER_PORT, const char *SOCKET_PATH>
std::string WebSocketService<SERVER_PORT, SOCKET_PATH>::rawMessage("");

template <int SERVER_PORT, const char *SOCKET_PATH>
int WebSocketService<SERVER_PORT, SOCKET_PATH>::messageCount(0);

// SINGLETON
// template <int SERVER_PORT, const char *SOCKET_PATH>
// WebSocketService<SERVER_PORT, SOCKET_PATH> *
//     WebSocketService<SERVER_PORT, SOCKET_PATH>::instance(NULL);

// template <int SERVER_PORT, const char *SOCKET_PATH>
// std::vector<WebSocketListener *> *
//     WebSocketService<SERVER_PORT, SOCKET_PATH>::wsListeners =
//         new std::vector<WebSocketListener *>();

template <int SERVER_PORT, const char *SOCKET_PATH>
WebSocketService<SERVER_PORT, SOCKET_PATH>::WebSocketService()
    : WebService<SERVER_PORT>() {
  WebService<SERVER_PORT>::serviceTreePath +=
      "\n    |__ [WebSocketService:" + std::to_string(SERVER_PORT) +
      SOCKET_PATH + "]";
  WebService<SERVER_PORT>::server.addHandler(&aws);
  aws.onEvent(eventHandler);
  // instance = this;
  std::cout << getServiceName() << " websocket listening on port "
            << SERVER_PORT << " path " << SOCKET_PATH << std::endl;
}

// template <int SERVER_PORT, const char *SOCKET_PATH>
// WebSocketService<SERVER_PORT, SOCKET_PATH> *
// WebSocketService<SERVER_PORT, SOCKET_PATH>::getSingleton() {
//   if (instance == nullptr) {
//     instance = new WebSocketService<SERVER_PORT, SOCKET_PATH>();
//     return instance;
//   } else
//     return instance;
// }

template <int SERVER_PORT, const char *SOCKET_PATH>
void WebSocketService<SERVER_PORT, SOCKET_PATH>::eventHandler(
    AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
    void *arg, uint8_t *data, size_t len) {
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
    forwardDispatch(arg, data, len, client->id());
    // dispatch(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

template <int WS_PORT, const char *WS_PATH>
void WebSocketService<WS_PORT, WS_PATH>::forwardDispatch(void *arg,
                                                         uint8_t *data,
                                                         size_t len,
                                                  int clientId) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    // convert raw data input
    std::string rawMsg(data, data + len);
    std::cout << "[WebSocketService] Forwarding message received from client #"
              << clientId << std::endl;
    /*** Forward message to all listeners ***/ 
    // singleton option
    // if (instance != nullptr)
    // instance->processMsg(rawMsg);
    // internal listeners option
    // for (auto wsl : *wsListeners) {
    //   wsl->processMsg(rawMsg);
    // }
    // external listeners option
    for (auto wsl : *WebSocketListener<WS_PORT, WS_PATH>::instances) {
      wsl->processMsg(rawMsg);
    }
    // external async listening option
    rawMessage = rawMsg;
    messageCount++;
    /*** Dispatch message to all connected clients ***/ 
    std::string replyMsg =
        ""; // "Message processed by " + std::to_string(wsListeners->size()) + "
            // wsservices";
    std::cout << "[WebSocketService > dispatch] reply to all connected clients "
              << replyMsg << std::endl;
    // notify server response to all connected clients
    // String dataOut(replyMsg.c_str());
    // aws.textAll(dataOut);
  }
}

// internal handler to be overriden in children class 
// template <int SERVER_PORT, const char *SOCKET_PATH>
// std::string
// WebSocketService<SERVER_PORT, SOCKET_PATH>::processMsg(std::string rawMsg) {
//   std::cout << "[WebSocketService:" << SERVER_PORT << SOCKET_PATH
//             << "] <processMsg> " << rawMsg << std::endl;
//   return std::string("msg handler Not Implemented");
// }

template <int SERVER_PORT, const char *SOCKET_PATH>
std::string WebSocketService<SERVER_PORT, SOCKET_PATH>::getServiceName() {
  return std::string("WebSocketService");
}

/**
 * WebSocketListener
 **/

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

template <int WS_PORT, const char *WS_PATH>
std::string
WebSocketListener<WS_PORT, WS_PATH>::processMsg(std::string rawMsg) {
  std::cout << "[WebSocketListener > processMsg]" << rawMsg << std::endl;
  return std::string("msg handler Not Implemented");
}
