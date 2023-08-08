#include <FsUtils.h>
#include <LittleFS.h>
#include <LogConsumers.h>
#include <iostream>

/*
 * Base class LogConsumer
 */

// std::vector<LogConsumer *> LogConsumer::instances;
std::vector<LogConsumer *> *LogConsumer::instances =
    new std::vector<LogConsumer *>();

LogConsumer::LogConsumer() {
  // LogConsumer::instances.push_back(this);
  LogConsumer::instances->push_back(this);
}

void LogConsumer::notifyAll(std::string logMsg) {
  // for (auto logConsumer : LogConsumer::instances) {
  //   logConsumer->onLog(logMsg);
  // }
}

void LogConsumer::onLog(std::string logMsg) {
  LogStore::info("[LogConsumer::onLog] new log notification ");
}

/*
 * child class TestLogConsumer
 */
TestLogConsumer::TestLogConsumer() : LogConsumer() {
  LogStore::info("[TestLogConsumer::constructor] create singleton");
}

TestLogConsumer &TestLogConsumer::instance() {
  // check if service already exists, otherwise create one
  static TestLogConsumer singleton; // this automatically add to LogConsumers
  return singleton;
}

/*
 * child class SerialLogConsumer
 */

SerialLogConsumer::SerialLogConsumer() {}

void SerialLogConsumer::init() {}

void SerialLogConsumer::printLog(std::string logMsg) {
  // log+"\r\n"
  cout << logMsg << endl;
}

SerialLogConsumer &SerialLogConsumer::instance() {
  // check if service already exists, otherwise create one
  static SerialLogConsumer singleton; // this automatically add to LogConsumers
  return singleton;
}

// overrides default impl from base class
void SerialLogConsumer::onLog(std::string logMsg) {
  LogStore::info("[SerialLogConsumer::onLog] log notification received ");
  SerialLogConsumer::printLog(logMsg);
  // SerialLogConsumer::printLog(LogStore::jsonExport());
}

/*
 * child class FsLogConsumer
 */

FsLogConsumer::FsLogConsumer() {}

void FsLogConsumer::init() {
  // move previous logs
  renameFile(LittleFS, LOG_FILE, OLD_LOG_FILE);
  // create new logfile
  writeFile(LittleFS, LOG_FILE, LogStore::jsonExport().c_str());
  // readFile(LITTLEFS, "/hello.txt");
}

std::string FsLogConsumer::readPastLog() {
  // log+"\r\n"
  std::string fileContent = readFile(LittleFS, OLD_LOG_FILE);
  return fileContent;
}

void FsLogConsumer::writeLog(std::string logMsg) {
  // log+"\r\n"
  appendFile(LittleFS, LOG_FILE, logMsg.c_str());
}

FsLogConsumer &FsLogConsumer::instance() {
  // check if service already exists, otherwise create one
  static FsLogConsumer singleton; // this automatically add to LogConsumers
  return singleton;
}

// overrides default impl from base class
void FsLogConsumer::onLog(std::string logMsg) {
  LogStore::info("[FsLogConsumer::onLog] log notification received ");
  // FsLogConsumer::writeLog(LogStore::jsonExport());
}