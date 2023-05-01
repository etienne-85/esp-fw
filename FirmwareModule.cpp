#include <Arduino.h>
#include <FirmwareModule.h>
#include <string>
// #include <FirmwareBase.h>

// class FirmwareModule
// {
int FirmwareModule::cycleCounter = 0;
std::vector<FirmwareModule *> *FirmwareModule::instances = new std::vector<FirmwareModule *>();

// int internalCycleCount = 0;
// int resetCycleCount = pow(2, 10); // the numbers of cycles after which internal counter is increased and cycleCounter is reset

FirmwareModule::FirmwareModule(std::string name) : moduleName(name)
{
    // FirmwareModule::cycleCounter = 0;
    FirmwareModule::instances->push_back(this);
}

void FirmwareModule::onCycle()
{
    internalCycleCount++; // increase internal counter
}

void FirmwareModule::setup()
{
}

void FirmwareModule::loop()
{
    if (cycleCounter++ >= resetCycleCount)
    {
        cycleCounter = 0; // reset counter
        onCycle();
    }
}

void FirmwareModule::setupAll()
{
    std::string logStr("");
    Serial.println("");
    Serial.println("[Firmware] Loading ...");
    std::size_t total = FirmwareModule::instances->size();
    logStr = "[Firmware] found " + std::to_string(total) + " modules ";
    Serial.println(logStr.c_str());
    Serial.println("");
    int i = 0;
    for (auto module : *FirmwareModule::instances)
    {
        // logStr = "[Module " + std::to_string(i + 1) + "/" + std::to_string(total) + "] ";
        logStr = "*** " + module->moduleName + " ***";
        Serial.println(logStr.c_str());
        module->setup();
        Serial.println("");
        i++;
    }
    Serial.println("");
    logStr = "[Firmware] Loaded " + std::to_string(i) + " modules";
    Serial.println(logStr.c_str());
    Serial.println("[Firmware] Running ...");
}

void FirmwareModule::loopAll()
{
    for (auto elem : *FirmwareModule::instances)
    {
        elem->loop();
    }
}
// };
