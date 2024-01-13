#pragma once
#include <LogStore.h>
#include <FileHandlers.h>
#include <defaults.h>
#include <string>

/*
 * Any class inheriting LogConsumer and overriding onLog
 * will be notified of new logs from LogStore
 */
class LogConsumer {
  // static std::vector<LogConsumer *> instances;
  static std::vector<LogConsumer *> *instances;

public:
  static void notifyAll(std::string logMsg);
  LogConsumer();
  virtual void onLog(std::string logMsg);
};

class TestLogConsumer : public LogConsumer {
  static TestLogConsumer &singleton;

public:
  static TestLogConsumer &instance();
  TestLogConsumer();
};

class SerialLogConsumer : public LogConsumer {
  static SerialLogConsumer &singleton;

public:
  static SerialLogConsumer &instance();
  static void init();
  static void printLog(std::string);
  SerialLogConsumer();
  virtual void onLog(std::string logMsg);
};

class FsLogConsumer : public LogConsumer {
  static JsonFileHandler &logs;
  static JsonFileHandler &logArchive;
  static FsLogConsumer &singleton;

public:
  static FsLogConsumer &instance();
  static void init();
  static void writeLog(std::string);
  static std::string readPastLog();
  FsLogConsumer();
  virtual void onLog(std::string logMsg);
};