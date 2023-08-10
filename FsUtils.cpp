#include <FsUtils.h>
#include <LogStore.h>

std::string printDate(struct tm *tmstruct) {
  return (std::to_string((tmstruct->tm_year) + 1900) + "-" +
          std::to_string(tmstruct->tm_mon + 1) + "-" +
          std::to_string(tmstruct->tm_mday) + " " +
          std::to_string(tmstruct->tm_hour) + ":" +
          std::to_string(tmstruct->tm_min) + ":" +
          std::to_string(tmstruct->tm_sec));
}

void FsUtils::listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    time_t t = file.getLastWrite();
    struct tm *tmstruct = localtime(&t);
    std::string datePrint("");
    datePrint += file.isDirectory()
                     ? "[dir] "
                     : "[file] " + std::to_string(file.size()) + "  ";
    datePrint += printDate(tmstruct) + "  " + std::string(file.name());
    LogStore::info(datePrint);

    if (levels) {
      listDir(fs, file.name(), levels - 1);
    }
    file = root.openNextFile();
  }
}

void FsUtils::createDir(fs::FS &fs, const char *path) {
  LogStore::info("Creating Dir: " + std::string(path));
  if (fs.mkdir(path)) {
    LogStore::info("Dir created");
  } else {
    LogStore::info("mkdir failed");
  }
}

void FsUtils::removeDir(fs::FS &fs, const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

std::string FsUtils::readFile(fs::FS &fs, const char *path) {
  LogStore::info("Reading file: " + std::string(path));

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    LogStore::info("- failed to open file for reading");
    return NULL;
  }

  LogStore::info("- read from file:");
  String fileContent = "";
  while (file.available()) {
    fileContent += (char)file.read();
  }
  file.close();
  std::string sFileContent(fileContent.c_str());
  return sFileContent;
}

void FsUtils::writeFile(fs::FS &fs, const char *path, const char *message) {
  LogStore::info("Writing file: " + std::string(path));

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    LogStore::info("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    LogStore::info("- file written");
  } else {
    LogStore::info("- write failed");
  }
  file.close();
}

void FsUtils::appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void FsUtils::renameFile(fs::FS &fs, const char *path1, const char *path2) {
  LogStore::info("Renaming file: " + std::string(path1) + " to " +
                 std::string(path2));
  if (fs.rename(path1, path2)) {
    LogStore::info("- file renamed");
  } else {
    LogStore::info("- rename failed");
  }
}

void FsUtils::deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

// SPIFFS-like write and delete file, better use #define
// CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void FsUtils::writeFile2(fs::FS &fs, const char *path, const char *message) {
  if (!fs.exists(path)) {
    if (strchr(path, '/')) {
      Serial.printf("Create missing folders of: %s\r\n", path);
      char *pathStr = strdup(path);
      if (pathStr) {
        char *ptr = strchr(pathStr, '/');
        while (ptr) {
          *ptr = 0;
          fs.mkdir(pathStr);
          *ptr = '/';
          ptr = strchr(ptr + 1, '/');
        }
      }
      free(pathStr);
    }
  }

  Serial.printf("Writing file to: %s\r\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void FsUtils::deleteFile2(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file and empty folders on path: %s\r\n", path);

  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }

  char *pathStr = strdup(path);
  if (pathStr) {
    char *ptr = strrchr(pathStr, '/');
    if (ptr) {
      Serial.printf("Removing all empty folders on path: %s\r\n", path);
    }
    while (ptr) {
      *ptr = 0;
      fs.rmdir(pathStr);
      ptr = strrchr(pathStr, '/');
    }
    free(pathStr);
  }
}

void FsUtils::testFileIO(fs::FS &fs, const char *path) {
  Serial.printf("Testing file I/O with %s\r\n", path);

  static uint8_t buf[512];
  size_t len = 0;
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }

  size_t i;
  Serial.print("- writing");
  uint32_t start = millis();
  for (i = 0; i < 2048; i++) {
    if ((i & 0x001F) == 0x001F) {
      Serial.print(".");
    }
    file.write(buf, 512);
  }
  Serial.println("");
  uint32_t end = millis() - start;
  Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
  file.close();

  file = fs.open(path);
  start = millis();
  end = start;
  i = 0;
  if (file && !file.isDirectory()) {
    len = file.size();
    size_t flen = len;
    start = millis();
    Serial.print("- reading");
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      if ((i++ & 0x001F) == 0x001F) {
        Serial.print(".");
      }
      len -= toRead;
    }
    Serial.println("");
    end = millis() - start;
    Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
    file.close();
  } else {
    Serial.println("- failed to open file for reading");
  }
}