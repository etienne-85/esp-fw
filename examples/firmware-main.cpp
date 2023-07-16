#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <ConfigLoader.h>
#include <FirmwareModule.h>
#include <WebServer.h>
#include <filesys-module.h>
#include <iostream>
#include <network-module.h>
// #include <WebSocketListener.h>
#include <RemoteGpioService.h>
#include <RemoteLogService.h>
#include <RemoteFsService.h>
#include <web-services-core.h>
// #include <module-template.h>
#include <CyclesCounter.h>
#include <LogStore.h>

// Core modules
FilesysModule filesysModule;
ConfigLoader confLoader;
NetworkModule networkModule;
// Test module
// TemplateModule testModule;

// Core services
// WebSocketListener *wsl;

/**
 * setup
 */
void setup() {
  Serial.begin(115200);
  LogStore::info("*******************************");
  LogStore::info("*** ESP32 Firmware build: d ***");
  LogStore::info("*******************************");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Core modules
  LogStore::info("[Setup] Init core modules ");
  FirmwareModule::setupAll();
  LogStore::info("[Setup] Start core services ");
  WebServer::instance().init();
  // GpioRemoteService::instance().init();
  GpioRemoteService::init();
  LogRemoteService::init();
  FsRemoteService::init();
  WebServer::instance().start();
  // StaticServer staticServer;
  // staticServer.init();
  OTAServiceWrapper otaService;
  otaService.init();
  // wsl = WebSocketListener::instance("/test");
  // ChatHandler::init();
  LogStore::info("[Setup] Done");
}

/**
 * main loop
 */
void loop() {
  // wsl->webSocket.cleanupClients();
  // WebSocketListener<80, wsPath>::asyncListenForwardLoop();
  // Refresh modules and services
  FirmwareModule::loopAll(); // core
  WebServer::instance().loop();
  // ChatHandler::loop();
  // GpioRemoteService::instance().loop(); // gpio service
  CyclesCounter::inc();
}