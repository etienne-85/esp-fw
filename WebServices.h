#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <string>
#include <vector>

/***
* WebServer
* extend it to provide additional server routes or services for a specific port
*/

template <int SERVER_PORT> 
class WebService {
public:
  // server at given port shared by all instances
  static AsyncWebServer server;
  // each instances providing custom routes for the shared server
  static std::vector<WebService<SERVER_PORT> *> *instances;
  // States @todo: support more states like running, stopped, failed..
  static bool serverState;
  bool state = false; // (child) service state

  std::string name();
  int index = 0;

  WebService();

  void start();

  void print();
};


/***
 * WebSocketService
 * each instance of this class can provide handlers for WS
 */

template <int SERVER_PORT, const char *SOCKET_PATH> 
class WebSocketService {
public:
  //
  static std::vector<WebSocketService<SERVER_PORT, SOCKET_PATH> *> *instances;
  // Web socket shared by all instances
  static AsyncWebSocket aws;

  std::string name();
  int index = 0;

  WebSocketService();

  void dispatch(void *arg, uint8_t *data, size_t len);

  void eventHandler(AsyncWebSocket *server,
                                      AsyncWebSocketClient *client,
                                      AwsEventType type, void *arg,
                                      uint8_t *data, size_t len);

  std::string processMsg(std::string rawMsg);
};
