#pragma once
// #include <iostream>
#include <Arduino.h>
#include <LittleFS.h>

class FsUtils {
public:
  static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

  static void createDir(fs::FS &fs, const char *path);

  static void removeDir(fs::FS &fs, const char *path);

  static std::string readFile(fs::FS &fs, const char *path);

  static void writeFile(fs::FS &fs, const char *path, const char *message);
  static void appendFile(fs::FS &fs, const char *path, const char *message);

  static void renameFile(fs::FS &fs, const char *path1, const char *path2);

  static void deleteFile(fs::FS &fs, const char *path);

  // SPIFFS-like write and delete file, better use #define
  // CONFIG_LITTLEFS_SPIFFS_COMPAT 1

  static void writeFile2(fs::FS &fs, const char *path, const char *message);

  static void deleteFile2(fs::FS &fs, const char *path);

  static void testFileIO(fs::FS &fs, const char *path);
};
