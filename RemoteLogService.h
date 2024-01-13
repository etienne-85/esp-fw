/**
 * Web socket service providing remote logs monitoring
 * Usage example from browser
 */

#include <ArduinoJson.h>
#include <HTTPSServer.hpp>
#include <LogConsumers.h>
#include <LogStore.h>
#include <WebServer.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>
#include <string>

// #define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

// The HTTPS Server comes in a separate namespace. For easier use, include it
// here.
using namespace httpsserver;

/*
 * All connected client to the service have dedicated ws handler
 * but share same service
 */
class LogRemoteService : public WebsocketHandler {
  static WebsocketNode *webSocketNode;
  static std::map<int, LogRemoteService *> instances;
  // this will be called each time new client connects to the /logs websocket
  static WebsocketHandler *onCreate();

public:
  static void init();

private:
  LogRemoteService(int clientId);
  int clientId = 0;

  // making service available at /logs
  // GpioRemoteService() : WebsocketNode("/chat", &GpioRemoteService::create);
  // GpioRemoteService(): WebsocketNode("/gpio", &GpioRemoteService::instance);

public:
  // overiding default base class method
  // void unpackMsg(std::string rawMsg);

  // static GpioRemoteService &instance();
  // static WebsocketHandler *instancePtr();

  // This method is called when a message arrives
  void onMessage(WebsocketInputStreambuf *input);
  void extractMsg(std::string rawMsg);
  void flushLogs();
  void dumpArchive();
  // Handler function on connection close
  void onClose();
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<int, LogRemoteService *> LogRemoteService::instances;

WebsocketNode *LogRemoteService::webSocketNode =
    new WebsocketNode("/logs", &LogRemoteService::onCreate);

// each time new client connects to the /gpio websocket a new service
// instance is created
WebsocketHandler *LogRemoteService::onCreate() {
  int clientNb = LogRemoteService::instances.size();
  LogRemoteService *instance = NULL;
  if (clientNb < MAX_CLIENTS) {
    instance = new LogRemoteService(clientNb);
    LogRemoteService::instances.insert({clientNb, instance});
  } else {
    LogStore::info(
        "[LogRemoteService::onCreate] max number of client reached #" +
        std::to_string(clientNb));
  }
  // will be cast to WebsocketHandler*
  return instance;
}

// LogRemoteService &LogRemoteService::instance() {
//   static LogRemoteService singleton;
//   return singleton;
// }

// WebsocketHandler *LogRemoteService::instancePtr() {
//   return &LogRemoteService::instance();
// }

LogRemoteService::LogRemoteService(int clientId)
    : WebsocketHandler(), clientId(clientId) {
  LogStore::info("[LogRemoteService] Client #" + std::to_string(clientId) +
                 " connected");
}

// Finally, passing messages around. If we receive something, we send it to all
// other clients
void LogRemoteService::onMessage(WebsocketInputStreambuf *msgBuff) {
  // Get the input message
  std::ostringstream ss;
  std::string incomingMsg;
  ss << msgBuff;
  incomingMsg = ss.str();
  // this->send("[LogRemoteService::onMessage]", SEND_TYPE_TEXT);
  //   pong(msg);
  // dump();
  extractMsg(incomingMsg);
}

// When the websocket is closing, we remove the client from the array
void LogRemoteService::onClose() {
  LogStore::info("[LogRemoteService] Client #" + std::to_string(clientId) +
                 " disconnected");
  // for(int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] == this) {
  //     activeClients[i] = nullptr;
  //   }
  // }
}

void LogRemoteService::extractMsg(std::string rawMsg) {
  StaticJsonDocument<JSON_MSG_SIZE> root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, rawMsg);

  // root level props
  std::string cmd = root["cmd"]; // for operation not tied to any pin
  // int msgRefId = root["msgRefId"]; // in case reply is needed

  // LogStore::info("[GpioRemoteService::unpackMsg] msgRefID " +
  //                std::to_string(msgRefId));

  // std::string replyMsg("Default reply from GpioRemoteService");

  if (cmd == "archived") {
    dumpArchive();
  } else {
    flushLogs();
  }
}

/*
* Flushing latest logs
*/
void LogRemoteService::flushLogs() {
  // std::cout << "[LogRemoteService] received message: " << msg << std::endl;

  // Send it back to every client
  // for (int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] != nullptr) {
  //     activeClients[i]->send(msg, SEND_TYPE_TEXT);
  //   }
  // }
  // aggregate all logs stored in buffer
  // std::string logs("[LogRemoteService] OK");

  this->send(LogStore::jsonExport(), SEND_TYPE_TEXT);
  // clear log buffer
  LogStore::logBuffer.clear();
}

/*
* Dumping full logs from previous boot
*/
void LogRemoteService::dumpArchive() {
  std::string rawContent = FsLogConsumer::readPastLog();
  StaticJsonDocument<200> jsData;
  deserializeJson(jsData, rawContent);
  // reserialize back
  std::string sData = "";
  serializeJsonPretty(jsData, sData);
  this->send(sData, SEND_TYPE_TEXT);
}

void LogRemoteService::init() {
  LogStore::info("[LogRemoteService::init]");
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}
