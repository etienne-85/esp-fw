#include <FsLogService.h>
#include <FsUtils.h>
#include <LittleFS.h>

void FsLogService::init() {
  // remove previous logs file
  deleteFile(LittleFS, OLD_LOG_FILE);
  // move previous logs
  renameFile(LittleFS, LOG_FILE, OLD_LOG_FILE);
  // create new logfile
  writeFile(LittleFS, LOG_FILE, "");
  // readFile(LITTLEFS, "/hello.txt");
}

std::string FsLogService::readPreviousLog() {
  // log+"\r\n"
  std::string fileContent = readFile(LittleFS, CONFIG_FILE);
  return fileContent;
}

void FsLogService::writeLog(std::string log) {
  // log+"\r\n"
  appendFile(LittleFS, LOG_FILE, log.c_str());
}