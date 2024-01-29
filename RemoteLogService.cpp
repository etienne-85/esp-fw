#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <RemoteLogService.h>
#include <WsRemoteInterface.h>

/**************************
 *** STATIC DEFINITIONS ***
 **************************/

LogRemoteService &LogRemoteService::instance() {
  static LogRemoteService singleton;
  return singleton;
}

LogRemoteService::LogRemoteService() : RemoteServiceListener(SERVICE_NAME) {
  LogStore::info("[LogRemoteService] constructor");
}

// void LogRemoteService::extractMsg(std::string rawMsg) {
std::string LogRemoteService::onServiceCall(std::string rawMsg, int clientId) {
  LogStore::dbg("[LogRemoteService::onServiceCall] " + rawMsg);

  JsonDocument root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, rawMsg);

  // root level props
  JsonObject dataIn = root["svcIn"];
  std::string event = dataIn["evt"];

  if (event == "subscribe") {
    return subscribe(clientId);
  } else if (event == "logbuff") {
    int logLevel = dataIn["level"];
    return logBuffer(logLevel);
  } else if (event == "logchunk") {
    int logId = dataIn["logId"];
    return logBuffer(logId);
  } else if (event == "archive") {
    int logLevel = dataIn["level"];
    return logArchive(logLevel);
  }
}

/*
 * Allowing client to be notified of new logs without having to poll regularly
 TODO: support min delay config between logs notification
 TODO#2: confirm subscription
 svcOut: {
  event: "subscribe"
  status: "confirmed"
 }
 */
//
//
std::string LogRemoteService::subscribe(int clientId) {
  if (clientId != -1) {
    LogStore::info(
        "[LogRemoteService::subscribe] subscription received from client " +
        std::to_string(clientId));
    std::string key(std::to_string(clientId));
    if (subscribers[key].isNull()) {
      JsonDocument newSub;
      newSub["clientId"] = clientId;
      subscribers[key] = newSub;
      LogStore::info("[LogRemoteService::subscribe] client " + key +
                     " will now receive log notifications");
      // TODO reply with confirmed status
    } else {
      LogStore::info("[LogRemoteService::subscribe] client " + key +
                     " already subscribed to log notifications");
      // TODO reply with nothing to do status
    }
  } else {
    LogStore::info("[LogRemoteService::subscribe] missing clientId ");
    // TODO reply with error status
  }
  return "";
}

// to be called anytime a client disconnect
std::string LogRemoteService::unsubscribe(int clientId) {
  std::string clientKey(std::to_string(clientId));
  JsonObject sub = LogRemoteService::instance().subscribers[clientKey];
  if (sub.isNull()) {
    LogStore::info("[LogRemoteService::subscribe] client " +
                   std::to_string(clientId) + " not in log subscribers");

  } else {
    LogStore::info("[LogRemoteService::subscribe] removing client " +
                   std::to_string(clientId) + " from log notifications");
  }
}

/*
 * Flushing latest logs
 */
std::string LogRemoteService::logBuffer(int logLevel) {
  LogStore::info("[LogRemoteService::logBuffer] ");
  std::string logExport(LogStore::jsonExport(logLevel));
  // clear log buffer
  LogStore::logBuffer.clear();
  return logExport;
}

/*
 * Single log line
 */
std::string LogRemoteService::logChunk(int logId) {
  LogStore::info("[LogRemoteService::logChunk] log id " + logId);
  return "NOT IMPLEMENTED";
}

/*
 * Dumping logs from previous boot
 */
std::string LogRemoteService::logArchive(int logLevel) {
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

// overrides default impl from base class
void LogRemoteService::onLog(std::string logMsg) {
  WsRemoteInterface::notifyClient(0, logMsg);
  // std::string dumpSubs("");
  // serializeJson(subscribers, dumpSubs);
  // LogStore::dbg("[LogRemoteService::subscribe] subs dump: " + dumpSubs);
  // if (subscribers != nullptr) {
  // for (JsonPair kv :
  //      LogRemoteService::instance().subscribers.as<JsonObject>()) {
  //   // key/value
  //   int clientKey = std::stoi(kv.key().c_str());
  //   LogStore::forget("DBG client key: " + std::to_string(clientKey));
  //   // extract subscription data
  //   // JsonObject subsConf = kv.value().as<JsonObject>();
  //   // notify client
  //   // WsRemoteInterface::notifyClient(clientKey, logMsg);
  // }
  // }
}