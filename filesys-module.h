#include <FirmwareModule.h>
#include <LittleFS.h>
#include <FsUtils.h>

class FilesysModule : public FirmwareModule
{
public:
    FilesysModule() : FirmwareModule("Filesystem")
    {
    }

    void setup()
    {
        FirmwareModule::setup();

        if (!LittleFS.begin(true))
        {
            Serial.println("[LittleFS] error while mounting filesystem");
        }
        Serial.println("[LittleFS] mounted successfully!");

        listDir(LittleFS, "/", 0);

        Serial.println("[LittleFS] Done");
    }
};