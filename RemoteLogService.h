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
#include <string>

#define SERVICE_NAME "logs"

// The HTTPS Server comes in a separate namespace. For easier use, include it
// here.
using namespace httpsserver;

/*
 * All connected client to the service have dedicated ws handler
 * but share same service
 */
class LogRemoteService : public RemoteServiceListener {
public:
  static LogRemoteService &instance();
  // static WebsocketNode *webSocketNode;
  // static std::map<int, LogRemoteService *> instances;
  // this will be called each time new client connects to the /logs websocket
  // static WebsocketHandler *onCreate();
  // static void init();

private:
  // LogRemoteService(int clientId);
  LogRemoteService();
  // int clientId = 0;

  // making service available at /logs
  // GpioRemoteService() : WebsocketNode("/chat", &GpioRemoteService::create);
  // GpioRemoteService(): WebsocketNode("/gpio", &GpioRemoteService::instance);

public:
  // overiding default base class method
  // void unpackMsg(std::string rawMsg);

  // static GpioRemoteService &instance();
  // static WebsocketHandler *instancePtr();

  // void extractMsg(std::string rawMsg);
  std::string processMsg(std::string rawMsg);

  std::string logBuffer();
  std::string logArchive();
  // // This method is called when a message arrives
  // void onMessage(WebsocketInputStreambuf *input);
  // // Handler function on connection close
  // void onClose();
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
// std::map<int, LogRemoteService *> LogRemoteService::instances;

// WebsocketNode *LogRemoteService::webSocketNode =
//     new WebsocketNode("/logs", &LogRemoteService::onCreate);

LogRemoteService &LogRemoteService::instance() {
  static LogRemoteService singleton;
  return singleton;
}

// WebsocketHandler *LogRemoteService::instancePtr() {
//   return &LogRemoteService::instance();
// }

// LogRemoteService::LogRemoteService(int clientId)
//     : WebsocketHandler(), clientId(clientId) {
//   LogStore::info("[LogRemoteService] Client #" + std::to_string(clientId) +
//                  " connected");
// }

LogRemoteService::LogRemoteService() : RemoteServiceListener(SERVICE_NAME) {
  LogStore::info("[LogRemoteService] constructor");
}

// void LogRemoteService::extractMsg(std::string rawMsg) {
std::string LogRemoteService::processMsg(std::string rawMsg) {
  LogStore::info("[LogRemoteService] processMsg " + rawMsg);

  JsonDocument root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, rawMsg);

  // root level props
  std::string cmd = root["cmd"];
  // int msgRefId = root["msgRefId"]; // in case reply is needed

  // LogStore::info("[GpioRemoteService::unpackMsg] msgRefID " +
  //                std::to_string(msgRefId));

  // std::string replyMsg("Default reply from GpioRemoteService");

  if (cmd == "logArchive") {
    return logArchive();
  } else {
    return logBuffer();
  }
}

/*
 * Flushing latest logs
 */
std::string LogRemoteService::logBuffer() {
  LogStore::info("[LogRemoteService::logBuffer] ");

  // Send it back to every client
  // for (int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] != nullptr) {
  //     activeClients[i]->send(msg, SEND_TYPE_TEXT);
  //   }
  // }
  // aggregate all logs stored in buffer
  // std::string logs("[LogRemoteService] OK");

  // this->send(LogStore::jsonExport(), SEND_TYPE_TEXT);
  std::string logExport(LogStore::jsonExport());
  // clear log buffer
  LogStore::logBuffer.clear();
  return logExport;
}

/*
 * Dumping logs from previous boot
 */
std::string LogRemoteService::logArchive() {
  LogStore::info("[LogRemoteService::logArchive] ");

  std::string rawContent = FsLogConsumer::readPastLog();
  JsonDocument jsData;
  deserializeJson(jsData, rawContent);
  // reserialize back
  std::string sData = "";
  serializeJsonPretty(jsData, sData);
  // this->send(sData, SEND_TYPE_TEXT);
  return sData;
}

// // each time new client connects to the /gpio websocket a new service
// // instance is created
// WebsocketHandler *LogRemoteService::onCreate() {
//   int clientNb = LogRemoteService::instances.size();
//   LogRemoteService *instance = NULL;
//   if (clientNb < MAX_CLIENTS) {
//     instance = new LogRemoteService(clientNb);
//     LogRemoteService::instances.insert({clientNb, instance});
//   } else {
//     LogStore::info(
//         "[LogRemoteService::onCreate] max number of client reached #" +
//         std::to_string(clientNb));
//   }
//   // will be cast to WebsocketHandler*
//   return instance;
// }

// // Finally, passing messages around. If we receive something, we send it to
// all
// // other clients
// void LogRemoteService::onMessage(WebsocketInputStreambuf *msgBuff) {
//   // Get the input message
//   std::ostringstream ss;
//   std::string incomingMsg;
//   ss << msgBuff;
//   incomingMsg = ss.str();
//   // this->send("[LogRemoteService::onMessage]", SEND_TYPE_TEXT);
//   //   pong(msg);
//   // dump();
//   extractMsg(incomingMsg);
// }

// // When the websocket is closing, we remove the client from the array
// void LogRemoteService::onClose() {
//   LogStore::info("[LogRemoteService] Client #" + std::to_string(clientId) +
//                  " disconnected");
//   // for(int i = 0; i < MAX_CLIENTS; i++) {
//   //   if (activeClients[i] == this) {
//   //     activeClients[i] = nullptr;
//   //   }
//   // }
// }

// void LogRemoteService::init() {
//   LogStore::info("[LogRemoteService::init]");
//   // register ws service to main secured server
//   WebServer::instance().secureServer.registerNode(webSocketNode);
// }
