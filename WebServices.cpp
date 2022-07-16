#include <WebServices.h>

/**
*   WebService
**/

template <int SERVER_PORT>
AsyncWebServer WebService<SERVER_PORT>::server(SERVER_PORT);

template <int SERVER_PORT>
std::vector<WebService<SERVER_PORT> *> *WebService<SERVER_PORT>::instances =
    new std::vector<WebService<SERVER_PORT> *>();

template <int SERVER_PORT> 
bool WebService<SERVER_PORT>::serverState = false;

template <int SERVER_PORT> 
WebService<SERVER_PORT>::WebService() {
  instances->push_back(this);
  index = instances->size() - 1;
  std::cout << "[WebService:" << SERVER_PORT << "]"
            << " new instance #" << index << std::endl;
}

template <int SERVER_PORT> 
void WebService<SERVER_PORT>::start() {
  if (!WebService<SERVER_PORT>::serverState) {
    std::cout << "[WebService:" << SERVER_PORT << "] Starting Server"
              << std::endl;
    // WebService<SERVER_PORT>::server = new AsyncWebServer(SERVER_PORT);
    WebService<SERVER_PORT>::server.begin();
    std::cout << "[WebService:" << SERVER_PORT << "] Server running on port "
              << SERVER_PORT << std::endl;
  }
}

template <int SERVER_PORT> 
void WebService<SERVER_PORT>::print() {
  std::cout << "[WebService: " << SERVER_PORT << "] "
            << "service #" << index << " (totalling " << instances->size()
            << " registered instances)" << std::endl;
}

/**
*   WebSocketService
**/

template <int SERVER_PORT, const char *SOCKET_PATH>
AsyncWebSocket WebSocketService<SERVER_PORT, SOCKET_PATH>::aws(SOCKET_PATH);

template <int SERVER_PORT, const char *SOCKET_PATH>
std::vector<WebSocketService<SERVER_PORT, SOCKET_PATH> *> *
    WebSocketService<SERVER_PORT, SOCKET_PATH>::instances =
        new std::vector<WebSocketService<SERVER_PORT, SOCKET_PATH> *>();

template <int SERVER_PORT, const char *SOCKET_PATH>
WebSocketService<SERVER_PORT, SOCKET_PATH>::WebSocketService() {
  instances->push_back(this);
  index = instances->size() - 1;
  std::cout << "[WebSocketService:" << SERVER_PORT << "/" << SOCKET_PATH << "]"
            << " new instance #" << index << std::endl;
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
    dispatch(arg, data, len);
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
    std::string replyMsg = processMsg(rawMsg);
    // notify server response to all connected clients
    String dataOut(replyMsg.c_str());
    aws.textAll(dataOut);
  }
}

// to be implemented in children classes
template <int SERVER_PORT, const char *SOCKET_PATH>
std::string WebSocketService<SERVER_PORT, SOCKET_PATH>::processMsg(std::string rawMsg) {
  
}
