#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <System.h>
#include <LogStore.h>
#include <LogConsumers.h>
#include <WebServer.h>
#include <web-services-core.h>
#include <CyclesCounter.h>
// Remote services
#include <RemoteServices.h>
// #include <RemoteFsService.h>
#include <RemoteGpioService.h>
#include <RemoteLogService.h>

/**
 * setup
 * Remote services hierarchy
 * - /ws
 *  -> gpio
 *  -> log
 *  -> adm
 *  -> mon
 *  -> fs
 */
void setup() {
  Serial.begin(115200);
  LogStore::info("**********************");
  LogStore::info("*** ESP32 Firmware ***");
  LogStore::info("**********************");
  LogStore::info("   BUILD ver: c");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  LogStore::info("*** System core init ***");
  System::coreInit();
  LogStore::info("");
  LogStore::info("*** Services ***");
  // TestLogConsumer::instance();
  // SerialLogConsumer::instance(); // only used to register as LogConsumer instance
  // SerialLogConsumer::init(); 
  // FsLogConsumer::instance(); // only used to register as LogConsumer instance
  // FsLogConsumer::init(); 
  WebServer::instance().init();
  // GpioRemoteService::instance().init();
  LogStore::info("");
  LogStore::info("*** Remote services ***");
  RemoteService::registerService("/ws", &RemoteService::registerClient);
  // RemoteService::registerVirtualService("/gpio", &GpioRemoteService::onMessage)
  // GpioRemoteService::init();
  // LogRemoteService::init();
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