#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <ConfigStore.h>
#include <CyclesCounter.h>
#include <LogConsumers.h>
#include <LogStore.h>
#include <System.h>
#include <WebServer.h>
#include <web-services-core.h>
// Remote services interfaces
#include <LoraRemoteInterface.h>
#include <WsRemoteInterface.h>
// Remote services
// #include <RemoteFsService.h>
#include <LoraRepeaterService.h>
// #include <LoraProxyService.h>
#include <RemoteGpioService.h>
#include <RemoteLogService.h>
#include <RemoteTestService.h>

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
  LogStore::info("   BUILD ver: CD");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  LogStore::info("*** System core init ***");
  System::coreInit();
  std::string deviceId = ConfigStore::configContent()["deviceId"];
  LogStore::info("DeviceID: " + deviceId);
  LogStore::info("");
  LogStore::info("*** Services ***");
  // TestLogConsumer::instance();
  // SerialLogConsumer::instance(); // only used to register as LogConsumer
  // instance SerialLogConsumer::init(); FsLogConsumer::instance(); // only used
  // to register as LogConsumer instance FsLogConsumer::init();
  WebServer::instance().init();
  // GpioRemoteService::instance().init();
  LogStore::info("");
  LogStore::info("*** Communication interfaces ***");
  WsRemoteInterface::registerDefaultServiceRoute();
  LoraRemoteInterface::instance().init();
  LogStore::info("*** Remote services ***");
  LogRemoteService::instance();
  GpioRemoteService::instance();
  // LoraProxyService::instance();
  LoraRepeaterService::instance();
  RemoteTestService::instance();
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
  LoraRemoteInterface::instance().listen();
  CyclesCounter::inc();
}