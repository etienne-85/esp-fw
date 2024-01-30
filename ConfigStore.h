#pragma once
#include <FileHandlers.h>
#include <LogStore.h>
#include <defaults.h>

class ConfigStore {
public:
  static JsonDocument configContent();
  static JsonDocument certKeyContent();
  static JsonDocument privateKeyContent();
};

JsonDocument ConfigStore::configContent() {
  static JsonFileHandler configFile(CONFIG_FILE);
  return configFile.jsFileContent;
}

JsonDocument ConfigStore::certKeyContent() {
  static JsonFileHandler certKeyFile(CERT_FILE);
  return certKeyFile.jsFileContent;
}

JsonDocument ConfigStore::privateKeyContent() {
  static JsonFileHandler privateKeyFile(PRIVATE_KEY_FILE);
  return privateKeyFile.jsFileContent;
}
