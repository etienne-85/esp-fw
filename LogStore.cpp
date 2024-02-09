#include <ArduinoJson.h>
#include <System.h>
#include <LogConsumers.h>
#include <LogStore.h>
#include <iostream>

std::map<int, LogData> LogStore::logBuffer;

void LogStore::add(string logMsg, int logLevel, bool bypassEvtQueue) {
  int logId = LogStore::logBuffer.size();
  LogData logData;
  logData.level = logLevel;
  logData.log = logMsg;
  logData.timestamp = System::time.elapsedTimeEstimate;
  LogStore::logBuffer.insert({logId, logData});
  // each time new log is added notify all log consumers through event
  if (!bypassEvtQueue) {
    LogConsumer::notifyAll(logMsg);
    pushEvent(logMsg, EventType::LogEvt);
    std::cout << logMsg << std::endl;
  }
  // non persistant logs only showing in console, skip log saving and
  // notification
  else {
    std::cout << logMsg << std::endl;
  }
}

void LogStore::info(string log, bool bypassEvtQueue) {
  LogStore::add(log, 0, bypassEvtQueue);
}

void LogStore::dbg(string log, bool bypassEvtQueue) {
  LogStore::add(log, 1, bypassEvtQueue);
}

std::string LogStore::jsonExport(int logLevel) {
  LogStore::dbg("[LogStore::jsonExport] ");
  JsonDocument jsData;
  JsonArray logs = jsData.createNestedArray("logs");
  for (auto const &item : LogStore::logBuffer) {
    LogData logData = item.second;
    // logs += logData.timestamp;
    if (logLevel == -1 || logData.level == logLevel) {
      logs.add(logData.log);
    }
  }
  std::string logExport("");
  serializeJsonPretty(jsData, logExport);
  return logExport;
}