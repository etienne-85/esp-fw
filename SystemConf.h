#pragma once
#include <ArduinoJson.h>

class SystemConf {
public:
  JsonDocument configContent();
  JsonDocument certKeyContent();
  JsonDocument privateKeyContent();
};
