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
// #include <module-template.h>

// Core modules
FilesysModule filesysModule;
ConfigLoader confLoader;
NetworkModule networkModule;

// Test module
// TemplateModule testModule;

// Core services
extern const char wsPath[] = "/ws"; // web socket path
WebSocketListener<80, wsPath> *wsl;
JsonWebSocketListener<80, wsPath, 200> *jwsl;


/**
 * setup
 */
void setup() {
  Serial.begin(115200);
  Serial.println("[ESP32] First log!");
  // AsyncWebSocket ws("/ws");// = new AsyncWebSocket(wsUrl.c_str());
  // // create and bind websocket event handler
  // WebSocketImpl wsh(&ws);
  // ws.onEvent(wsh.eventHandler);
  // webModule.addWebSocket(&ws);
  //  bind websocket event handler
  //   WebSocketService::aws.onEvent(WebSocketService::eventHandler);
  //   webModule.addWebSocket(&WebSocketService::aws);
  // webSrv1.print();
  // Core modules
  Serial.println("[ESP32] Init core modules ");
  FirmwareModule::setupAll();
  Serial.println("[ESP32] Start core services ");
  WebService<80>::start();
  StaticServer<80> staticServer;
  staticServer.init();
  OTAServiceWrapper<80> otaService;
  otaService.init();
  WebSocketService<80, wsPath> wss;
  wsl = new WebSocketListener<80, wsPath>();
  jwsl = new JsonWebSocketListener<80, wsPath, 200>();
  // CamService<80, wsPath> cs;
  Serial.println("[ESP32] Done loading services");
  Serial.println("[BuildTag] abcdef");
}

/**
 * main loop
 */
void loop() {
  WebSocketService<80, wsPath>::aws.cleanupClients();
  // WebSocketListener<80, wsPath>::asyncListenForwardLoop();
  // Refresh all core modules
  FirmwareModule::loopAll();
}