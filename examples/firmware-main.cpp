#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <LogConsumers.h>
#include <LogStore.h>
#include <System.h>
#include <WebServer.h>
#include <web-services-core.h>
// Remote services interfaces
#include <LoraInterface.h>
#include <WsInterface.h>
// Remote services
// #include <RemoteFsService.h>
// #include <MessageRepeaterService.h>
// #include <LoraProxyService.h>
#include <EventMessageNotifications.h>
// #include <RemoteGpioService.h>
#include <AlarmSystem.h>
#include <Notifications.h>
#include <TestModule.h>
#define PIR_PIN 15
#define BUZZER_PIN 16
#define BTN_PIN 17

RTC_DATA_ATTR int bootCount = 0;

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
  LogStore::info("**********************************");
  LogStore::info("*****     ESP32 Firmware     *****");
  LogStore::info("*****       BUILD: OP        *****");
  LogStore::info("**********************************");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  LogStore::info("\n*** SYSTEM ***");
  System::coreInit();
  std::string deviceId = System::cfgStore.configContent()["deviceId"];
  LogStore::info("DeviceID: " + deviceId);
  LogStore::info("");
  SndNotif::instance(BUZZER_PIN).bips(1, 10);
  // BuzAlert::instance(BUZZER_PIN);
  // LogStore::info("*** Services ***");
  // TestLogConsumer::instance();
  // SerialLogConsumer::instance(); // only used to register as LogConsumer
  // instance SerialLogConsumer::init(); FsLogConsumer::instance(); // only used
  // to register as LogConsumer instance FsLogConsumer::init();
  WebServer::instance().init();
  // GpioRemoteService::instance().init();
  LogStore::info("\n*** MESSAGE INTERFACES ***");
  WsInterface::registerDefaultServiceRoute();
  LoraInterface::instance().init();
  LogStore::info("\n*** API MODULES ***");
  // GpioRemoteService::instance();
  // LoraProxyService::instance();
  // MessageRepeaterService::instance();
  EventMessageNotifications::instance();
  TestModule::instance();
  TriggerPin pinObs(BTN_PIN);
  // AlarmSystem::instance();
  AlarmSystem *alarmSystem = new AlarmSystem(PIR_PIN, BUZZER_PIN);
  LogStore::info("\n*** SERVICES ***");
  WebServer::instance().start();
  // StaticServer staticServer;
  // staticServer.init();
  OTAServiceWrapper otaService;
  otaService.init();
  std::cout << std::hex << 128;
  LogStore::info("\n*** READY ***\n");
}

/**
 * main loop
 */
void loop() {
  WebServer::instance().loop();
  LoraInterface::instance().listen();
  System::time.onCycle();
  EventQueue::watchEvents();
}