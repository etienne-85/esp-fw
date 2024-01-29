#include <ArduinoJson.h>
#include <CyclesCounter.h>
#include <LogConsumers.h>
#include <LogStore.h>
#include <iostream>

std::map<int, LogData> LogStore::logBuffer;

void LogStore::add(string logMsg, int logLevel) {
  int logId = LogStore::logBuffer.size();
  LogData logData;
  logData.level = logLevel;
  logData.log = logMsg;
  logData.timestamp = CyclesCounter::cycles;
  LogStore::logBuffer.insert({logId, logData});
  // each time new log is added notify all log consumers
  LogConsumer::notifyAll(logMsg);
  std::cout << logMsg << std::endl;
}

void LogStore::info(string log) { LogStore::add(log, 0); }

void LogStore::dbg(string log) { LogStore::add(log, 1); }

// non persistant logs only showing in console, skip log saving and notification
void LogStore::forget(string log) { std::cout << log << std::endl; }

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