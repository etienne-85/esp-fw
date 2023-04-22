#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "soc/soc.h"          // Disable brownour problems
#include <Arduino.h>
#include <iostream>
#include <ConfigLoader.h>
#include <FirmwareModule.h>
#include <filesys-module.h>
#include <network-module.h>
#include <WebServices.cpp> // need to use cpp instead of header here due to template use
#include <core-services.h>
// #include <GpioService.h>
// #include <module-template.h>

// Core modules
FilesysModule filesysModule;
ConfigLoader confLoader;
NetworkModule networkModule;
// Test module
// TemplateModule testModule;

// Core services
WebSocketListener<> *wsl;
JsonWebSocketListener<> *jwsl;


/**
 * setup
 */
void setup() {
  Serial.begin(115200);
  Serial.println("[ESP32] First log!");
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Core modules
  Serial.println("[ESP32] Init core modules ");
  FirmwareModule::setupAll();
  Serial.println("[ESP32] Start core services ");
  WebService<>::start();
  StaticServer<> staticServer;
  staticServer.init();
  OTAServiceWrapper<> otaService;
  otaService.init();
  WebSocketService<> wss;
  wsl = new WebSocketListener<>();
  jwsl = new JsonWebSocketListener<>();
  Serial.println("[ESP32] Done loading services");
  Serial.println("[BuildTag] abcde");
}

/**
 * main loop
 */
void loop() {
  WebSocketService<>::aws.cleanupClients();
  // WebSocketListener<80, wsPath>::asyncListenForwardLoop();
  // Refresh all core modules
  FirmwareModule::loopAll();
}