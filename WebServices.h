#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <string>
#include <vector>

class Service {
public:
  static std::vector<Service *> *instances;
  std::string serviceTreePath = "";
  Service();
  virtual std::string getServiceName() = 0;
  void printLog(std::string log);
  // void buildServiceTreePath();
  virtual void printServiceTree(int depth);
  static void printServiceTree(std::string dummyArg);
};

/***
 * WebServer
 * extend this class to provide additional server routes
 * or services for a specific port
 */

template <int SERVER_PORT> class WebService : public Service {
public:
  // server at given port shared by all instances
  static AsyncWebServer server;
  // each instances providing custom routes for the shared server
  // static std::vector<WebService<SERVER_PORT> *> *instances;
  // States @todo: support more states like running, stopped, failed..
  static bool serverState;
  bool state = false; // (child) service state

  WebService();
  static void start();
};

/***
 * WebSocketService
 * each instance of this class can provide handlers for WS
 */

template <int SERVER_PORT, const char *SOCKET_PATH>
class WebSocketService : public WebService<SERVER_PORT> {
public:
  // Web socket object associated to this singleton
  static AsyncWebSocket aws;

private:
  // Singleton
  static WebSocketService<SERVER_PORT, SOCKET_PATH> *instance;
  // protect constructor
  WebSocketService();

public:
  static WebSocketService<SERVER_PORT, SOCKET_PATH> *getSingleton();

  static void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client,
                           AwsEventType type, void *arg, uint8_t *data,
                           size_t len);

  static void dispatch(void *arg, uint8_t *data, size_t len);

  std::string processMsg(std::string rawMsg);
  std::string getServiceName();
};
