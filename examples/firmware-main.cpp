#include <Arduino.h>
#include <ConfigLoader.h>
#include <FirmwareModule.h>
#include <filesys-module.h>
#include <iostream>
#include <network-module.h>
#include <WebServices.cpp> // need to use cpp instead of header here due to template use
#include <core-services.h>
#include <module-template.h>

// Core modules
FilesysModule filesysModule;
ConfigLoader confLoader;
NetworkModule networkModule;

// Test module
TemplateModule testModule;

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
  // Services
  Serial.println("[ESP32] Start services ");
  StaticServer<80> staticServer;
  staticServer.start();
  OTAServiceWrapper<80> otaService;
  otaService.start();
  Serial.println("[ESP32] Done loading services");
}

/**
 * main loop
 */
void loop() {
  //   WebSocketService::aws.cleanupClients();
  // Refresh all modules
  FirmwareModule::loopAll();
}