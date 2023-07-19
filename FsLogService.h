#pragma once
#include <defaults.h>
#include <string>

class FsLogService {
public:
  static void init();
  static void writeLog(std::string);
  static std::string readPreviousLog();
};