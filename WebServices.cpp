#include <WebServices.h>
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
AsyncWebSocket WebSocketService<SERVER_PORT, SOCKET_PATH>::aws(SOCKET_PATH);

template <int SERVER_PORT, const char *SOCKET_PATH>
WebSocketService<SERVER_PORT, SOCKET_PATH> *
    WebSocketService<SERVER_PORT, SOCKET_PATH>::instance(NULL);

template <int SERVER_PORT, const char *SOCKET_PATH>
WebSocketService<SERVER_PORT, SOCKET_PATH>::WebSocketService()
    : WebService<SERVER_PORT>() {
  WebService<SERVER_PORT>::serviceTreePath +=
      "\n    |__ [WebSocketService:" + std::to_string(SERVER_PORT) + SOCKET_PATH + "]";
  WebService<SERVER_PORT>::server.addHandler(
      &WebSocketService<SERVER_PORT, SOCKET_PATH>::aws);
  WebSocketService<SERVER_PORT, SOCKET_PATH>::aws.onEvent(eventHandler);
  std::cout << getServiceName() << " websocket listening on port "
            << SERVER_PORT << " path " << SOCKET_PATH << std::endl;
}

template <int SERVER_PORT, const char *SOCKET_PATH>
WebSocketService<SERVER_PORT, SOCKET_PATH> *
WebSocketService<SERVER_PORT, SOCKET_PATH>::getSingleton() {
  if (WebSocketService<SERVER_PORT, SOCKET_PATH>::instance == nullptr) {
    WebSocketService<SERVER_PORT, SOCKET_PATH>::instance =
        new WebSocketService<SERVER_PORT, SOCKET_PATH>();
    return WebSocketService<SERVER_PORT, SOCKET_PATH>::instance;
  } else
    return WebSocketService<SERVER_PORT, SOCKET_PATH>::instance;
}

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
    WebSocketService<SERVER_PORT, SOCKET_PATH>::dispatch(arg, data, len);
    // dispatch(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

template <int SERVER_PORT, const char *SOCKET_PATH>
void WebSocketService<SERVER_PORT, SOCKET_PATH>::dispatch(void *arg,
                                                          uint8_t *data,
                                                          size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    // convert raw data input
    std::string rawMsg(data, data + len);
    // process message received from one client
    std::string replyMsg =
        WebSocketService<SERVER_PORT, SOCKET_PATH>::getSingleton()->processMsg(
            rawMsg);
    // notify server response to all connected clients
    String dataOut(replyMsg.c_str());
    aws.textAll(dataOut);
  }
}

// to be implemented in children classes
template <int SERVER_PORT, const char *SOCKET_PATH>
std::string
WebSocketService<SERVER_PORT, SOCKET_PATH>::processMsg(std::string rawMsg) {
  std::cout << "[WebSocketService:" << SERVER_PORT << "/" << SOCKET_PATH
            << "] <processMsg> "
            << "missing implementation ";
  return std::string("Not Implemented");
}

template <int SERVER_PORT, const char *SOCKET_PATH>
std::string WebSocketService<SERVER_PORT, SOCKET_PATH>::getServiceName() {
  return std::string("WebSocketService");
}
