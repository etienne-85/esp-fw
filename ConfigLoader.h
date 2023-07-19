#include <Arduino.h>
#include <ArduinoJson.h>
#include <FirmwareModule.h>
#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>
#include <WiFi.h>
#include <defaults.h>
#include <string>
// #include "../../../Dont-Commit-Me.h"
/**
 *   Loading config at runtime from an external json file
 */
class ConfigLoader : public FirmwareModule {

public:
  static StaticJsonDocument<200> jsonConfig;
  // if redefining constructor, call base class to register
  ConfigLoader() : FirmwareModule("ConfigLoader") { FirmwareModule::setup(); }

  /**
   *   Load config from external file stored on device FS
   */
  void setup() {
    LogStore::info("[ConfigLoader] loading");

    std::string fileContent = readFile(LittleFS, CONFIG_FILE);
    // convert to a json object
    DeserializationError error = deserializeJson(jsonConfig, fileContent);

    // Test if parsing succeeds.
    if (!error) {
      LogStore::info("[ConfigLoader] Done parsing configuration");
    } else {
      LogStore::info("deserializeJson() failed: ");
      Serial.println(error.f_str());
    }
  }
};

StaticJsonDocument<200> ConfigLoader::jsonConfig;
