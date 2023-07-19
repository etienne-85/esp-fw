#include <CyclesCounter.h>
#include <FsLogService.h>
#include <LogStore.h>
#include <ArduinoJson.h>
#include <iostream>

std::map<int, LogData> LogStore::logBuffer;

void LogStore::add(string logMsg, int logLevel) {
  int logId = LogStore::logBuffer.size();
  LogData logData;
  logData.level = logLevel;
  logData.log = logMsg;
  logData.timestamp = CyclesCounter::cycles;
  LogStore::logBuffer.insert({logId, logData});
  cout << logMsg << endl;
}

void LogStore::info(string log) { LogStore::add(log, 0); }

void LogStore::dbg(string log) { LogStore::add(log, 1); }

std::string LogStore::jsonExport() {
  StaticJsonDocument<5000> jsData;
  JsonArray logs = jsData.createNestedArray("logs");
  for (auto const &item : LogStore::logBuffer) {
    LogData logData = item.second;
    // logs += logData.timestamp;
    logs.add(logData.log);
  }
  std::string output("");
  serializeJsonPretty(jsData, output);
  return output;
}
