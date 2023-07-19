#include <FsUtils.h>
#include <LittleFS.h>
#include <LogStore.h>

class CoreModules {
public:
  static void initFs();
};

void CoreModules::initFs() {
  if (!LittleFS.begin(true)) {
    LogStore::info("[Core::FS] error while mounting filesystem");
  }
  LogStore::info("[Core::FS] mounted successfully!");

  listDir(LittleFS, "/", 0);

  LogStore::info("[Core::FS] Done");
}