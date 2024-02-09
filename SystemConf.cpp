#include <SystemConf.h>
#include <FileHandlers.h>
#include <LogStore.h>
#include <defaults.h>

JsonDocument SystemConf::configContent() {
  static JsonFileHandler configFile(CONFIG_FILE);
  return configFile.jsFileContent;
}

JsonDocument SystemConf::certKeyContent() {
  static JsonFileHandler certKeyFile(CERT_FILE);
  return certKeyFile.jsFileContent;
}

JsonDocument SystemConf::privateKeyContent() {
  static JsonFileHandler privateKeyFile(PRIVATE_KEY_FILE);
  return privateKeyFile.jsFileContent;
}