#pragma once
#include <Events.h>
#include <map>
#include <string>
#include <vector>
// struct enum LOG_LEVEL {
//     LOG_INFO,
//     LOG_DBG
// }
using namespace std;

struct LogData {
  int level;
  string log;
  int timestamp;
};

class LogStore {
public:
  static std::map<int, LogData> logBuffer;
  static void add(string log, int logLevel, bool bypassEvtQueue = false);

  static void info(string log);
  static void dbg(string log);

  static std::string jsonExport(int logLevel = -1);
};