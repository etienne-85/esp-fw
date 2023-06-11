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
#include <GpioRemoteService.h>
#include <web-services-core.h>
// #include <module-template.h>

// Core modules
FilesysModule filesysModule;
ConfigLoader confLoader;
NetworkModule networkModule;
// Test module
// TemplateModule testModule;

// Core services
WebSocketListener *wsl;

/**
 * setup
 */
void setup() {
  Serial.begin(115200);
  Serial.println("*******************************");
  Serial.println("*** ESP32 Firmware build: a ***");
  Serial.println("*******************************");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Core modules
  Serial.println("[Setup] Init core modules ");
  FirmwareModule::setupAll();
  Serial.println("[Setup] Start core services ");
  WebServer::getDefault().start();
  StaticServer staticServer;
  staticServer.init();
  OTAServiceWrapper otaService;
  otaService.init();
  // WebSocketService<> wss;
  // WebSocketService &wss = WebSocketService::instance("/test");
  wsl = WebSocketListener::instance("/test");
  GpioRemoteService::instance();
  Serial.println("[Setup] Done");
}

/**
 * main loop
 */
void loop() {
  wsl->webSocket.cleanupClients();
  // WebSocketListener<80, wsPath>::asyncListenForwardLoop();
  // Refresh all core modules
  FirmwareModule::loopAll();
}