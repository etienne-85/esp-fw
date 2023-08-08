#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <CoreModules.h>
#include <LogStore.h>
#include <LogConsumers.h>
#include <WebServer.h>
#include <web-services-core.h>
#include <CyclesCounter.h>
// Remote services
#include <RemoteFsService.h>
#include <RemoteGpioService.h>
#include <RemoteLogService.h>

/**
 * setup
 */
void setup() {
  Serial.begin(115200);
  LogStore::info("**********************");
  LogStore::info("*** ESP32 Firmware ***");
  LogStore::info("**********************");
  LogStore::info("   BUILD ver: a");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Core modules
  LogStore::info("");
  LogStore::info("*** Core modules ***");
  CoreModules::initFs();
  CoreModules::loadConfigFile();
  CoreModules::setupNetwork();
  LogStore::info("");
  LogStore::info("*** Core services ***");
  // TestLogConsumer::instance();
  // SerialLogConsumer::instance(); // only used to register as LogConsumer instance
  // SerialLogConsumer::init(); 
  // FsLogConsumer::instance(); // only used to register as LogConsumer instance
  // FsLogConsumer::init(); 
  WebServer::instance().init();
  // GpioRemoteService::instance().init();
  LogStore::info("");
  LogStore::info("*** Remote services ***");
  GpioRemoteService::init();
  LogRemoteService::init();
  // FsRemoteService::init();
  LogStore::info("");
  LogStore::info("*** Start services ***");
  WebServer::instance().start();
  // StaticServer staticServer;
  // staticServer.init();
  OTAServiceWrapper otaService;
  otaService.init();
  LogStore::info("*** Done ***");
  LogStore::info("");
}

/**
 * main loop
 */
void loop() {
  WebServer::instance().loop();
  CyclesCounter::inc();
}