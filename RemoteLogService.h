#pragma once

#include <ArduinoJson.h>
#include <LogConsumers.h>
#include <RemoteServiceListener.h>
#include <map>
#include <string>

#define SERVICE_NAME "logs"

/**
 * Web socket service providing remote logs monitoring
 */
class LogRemoteService : public RemoteServiceListener, public LogConsumer {
public:
  static LogRemoteService &instance();

private:
  LogRemoteService();

public:
  JsonDocument subscribers;
  // static std::map<int, JsonObject> subscribers;
  std::string onServiceCall(std::string rawMsg, int clientId = -1);

  void notify();
  std::string subscribe(int clientId);
  std::string unsubscribe(int clientId);
  std::string logChunk(int logId);
  std::string logBuffer(int logLevel);
  std::string logArchive(int logLevel);
  // overidding LogConsumer base class method
  virtual void onLog(std::string logMsg);
};