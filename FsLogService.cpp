#include <FsLogService.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>

void FsLogService::init() {
  // move previous logs
  renameFile(LittleFS, LOG_FILE, OLD_LOG_FILE);
  // create new logfile
  writeFile(LittleFS, LOG_FILE, LogStore::jsonExport().c_str());
  // readFile(LITTLEFS, "/hello.txt");
}

std::string FsLogService::readPreviousLog() {
  // log+"\r\n"
  std::string fileContent = readFile(LittleFS, OLD_LOG_FILE);
  return fileContent;
}

void FsLogService::writeLog(std::string log) {
  // log+"\r\n"
  appendFile(LittleFS, LOG_FILE, log.c_str());
}