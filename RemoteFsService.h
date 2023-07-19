/**
 * Web socket service providing remote filesystem operations
 */

#include <ArduinoJson.h>
#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <WebServer.h>
#include <WebsocketHandler.hpp>
#include <defaults.h>
#include <string>

// #define JSON_SIZE DEFAULT_JSON_SIZE
#define JSON_MSG_SIZE DEFAULT_JSON_SIZE

using namespace httpsserver;

class FsRemoteService : public WebsocketHandler {
  // STATIC FIELDS
  static WebsocketNode
      *webSocketNode; // ws route registration on secured webserver
  static std::map<int, FsRemoteService *>
      instances; // dedicated instance for each connected client
  // this will be called each time new client connects to the /logs websocket
  static WebsocketHandler *onCreate();

  // FIELDS
  int clientId = 0;

  // STATIC METHODS
public:
  static void init();
  // METHODS
private:
  FsRemoteService(int clientId);

public:
  // This method is called when a message arrives
  void onMessage(WebsocketInputStreambuf *input);
  // Handler function on connection close
  void onClose();
  // FS operations
  void createFile();
  void deleteFile();
  //   void openFile(std::string file);
  //   void closeFile();
  void readFile(std::string file);
  void writeFile();
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/
std::map<int, FsRemoteService *> FsRemoteService::instances;

WebsocketNode *FsRemoteService::webSocketNode =
    new WebsocketNode("/fs", &FsRemoteService::onCreate);

// each time new client connects to the /gpio websocket a new service
// instance is created
WebsocketHandler *FsRemoteService::onCreate() {
  int clientNb = FsRemoteService::instances.size();
  FsRemoteService *instance = NULL;
  if (clientNb < MAX_CLIENTS) {
    instance = new FsRemoteService(clientNb);
    FsRemoteService::instances.insert({clientNb, instance});
  } else {
    LogStore::info(
        "[FsRemoteService::onCreate] max number of client reached #" +
        std::to_string(clientNb));
  }
  // will be cast to WebsocketHandler*
  return instance;
}

/**************************
 *** INSTANCE DEFINITIONS ***
 **************************/

FsRemoteService::FsRemoteService(int clientId)
    : WebsocketHandler(), clientId(clientId) {
  LogStore::info("[FsRemoteService] new instance for client #" +
                 std::to_string(clientId));
}

void FsRemoteService::init() {
  LogStore::info("[FsRemoteService::init]");
  // register ws service to main secured server
  WebServer::instance().secureServer.registerNode(webSocketNode);
}

// Finally, passing messages around. If we receive something, we send it to all
// other clients
void FsRemoteService::onMessage(WebsocketInputStreambuf *inbuf) {
  // Get the input message
  std::ostringstream ss;
  std::string msg;
  ss << inbuf;
  msg = ss.str();
  // this->send("[LogRemoteService::onMessage]", SEND_TYPE_TEXT);
  readFile(CONFIG_FILE);
}

// When the websocket is closing, we remove the client from the array
void FsRemoteService::onClose() {
  // for(int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] == this) {
  //     activeClients[i] = nullptr;
  //   }
  // }
}

/*
 *   FS operations
 */

void FsRemoteService::createFile() {}
void FsRemoteService::deleteFile() {}
// void FsRemoteService::openFile(std::string file) {}
// void FsRemoteService::closeFile() {}
void FsRemoteService::readFile(std::string file) {
  LogStore::info("[FsRemoteService::readFile] file " + file);
  std::string fileContent = "TODO read file content"; //readFile(LittleFS, file);
  this->send(fileContent, SEND_TYPE_TEXT);
}
void FsRemoteService::writeFile() {
  // std::cout << "[LogRemoteService] received message: " << msg << std::endl;

  // Send it back to every client
  // for (int i = 0; i < MAX_CLIENTS; i++) {
  //   if (activeClients[i] != nullptr) {
  //     activeClients[i]->send(msg, SEND_TYPE_TEXT);
  //   }
  // }

  // aggregate all logs stored in buffer

  //   StaticJsonDocument<5000> doc;
  //   JsonArray logs = doc.createNestedArray("logs");
  //   for (auto const &item : LogStore::logBuffer) {
  //     LogData logData = item.second;
  //     // logs += logData.timestamp;
  //     logs.add(logData.log);
  //   }
  //   std::string output("");
  //   serializeJsonPretty(doc, output);
  //   this->send(output, SEND_TYPE_TEXT);
  //   // clear log buffer
  //   LogStore::logBuffer.clear();
}
