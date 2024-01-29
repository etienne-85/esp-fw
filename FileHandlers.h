#pragma once
#include <ArduinoJson.h>
#include <string>

class FileHandler {
public:
  std::string fileName;
  // STATIC METHODS
  //   static void init();
  // METHODS
  FileHandler(std::string fileName);

public:
  void createFile();
  void deleteFile();
  std::string readContent();
  void writeContent();
};

class JsonFileHandler : public FileHandler {
public:
  JsonDocument jsFileContent;

public:
  JsonFileHandler(std::string fileName);
  void load();
  void merge(std::string newContent);
  void flush();
};