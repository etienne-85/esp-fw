#pragma once
#include <FileHandlers.h>
#include <LogStore.h>
#include <defaults.h>

class ConfigStore {
  static const JsonFileHandler *configFile;
public:
  static const JsonFileHandler *certFile;
  static const JsonFileHandler *privateKeyFile;

public:
  static void init();
  static void initKeys();
  static std::string setting(std::string name);
};

const JsonFileHandler *ConfigStore::configFile = NULL;
const JsonFileHandler *ConfigStore::privateKeyFile = NULL;
const JsonFileHandler *ConfigStore::certFile = NULL;

void ConfigStore::init() {
  LogStore::info("[ConfigStore::init] loading main config file ");
  ConfigStore::configFile = new JsonFileHandler(CONFIG_FILE);
  // static JsonFileHandler configFile = JsonFileHandler(CONFIG_FILE);
}

void ConfigStore::initKeys() {
  LogStore::info("[ConfigStore::initKeys] loading private and cert keys");
  ConfigStore::privateKeyFile = new JsonFileHandler(PRIVATE_KEY_FILE);
  ConfigStore::certFile = new JsonFileHandler(CERT_FILE);
}

std::string ConfigStore::setting(std::string name) {
  return ConfigStore::configFile->jsFileContent[name.c_str()];
}
