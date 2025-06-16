#include <ArduinoJson.h>
#include <LogConsumers.h>
#include <LogStore.h>
#include <System.h>
#include <iostream>

std::map<int, LogData> LogStore::logBuffer;

void LogStore::add(std::string logMsg, int logLevel, bool bypassEvtQueue) {
  int logId = LogStore::logBuffer.size();
  int localTime = millis(); // device local elapsed time
  LogData logData;
  logData.level = logLevel;
  logData.log = logMsg;
  logData.timestamp = localTime; // System::time.elapsedTimeEstimate;
  LogStore::logBuffer.insert({logId, logData});
  // each time new log is added notify all log consumers through event
  if (!bypassEvtQueue) {
    // LogConsumer::notifyAll(logMsg);
    std::cout << logMsg << std::endl; // << "(" << std::to_string(localTime) << ") "
    std::string evtType = EventTypeMap[EventType::LOG];
    EventContext evtCtx; // use default
    // customise any required
    // evtCtx.origin = EventOrigin.LOCAL;
    // evtCtx.timestamp = 0;
    // evtCtx.priority = 0;
    Event evt;
    evt.type = evtType;
    evt.content = logMsg;
    evt.context = evtCtx;
    EventQueue::pushEvent(evt);
  }
  // non persistant logs only showing in console, skip log saving and
  // notification
  else {
    std::cout << logMsg << std::endl;
  }
}

void LogStore::info(string log) { LogStore::add(log, 0); }

void LogStore::dbg(string log) { LogStore::add(log, 1, true); }

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