#pragma once
#include <defaults.h>
#include <string>

class FsLogService {
  static void init();

public:
  static void writeLog(std::string);
  static std::string readPreviousLog();
};