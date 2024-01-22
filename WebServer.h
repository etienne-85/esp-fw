#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <HTTPSServer.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

using namespace httpsserver;

/***
 * WebServer
 * extend this class to provide additional server routes
 * or services for a specific port
 */

class WebServer { //: public Service {
public:
  static WebServer &singleton;
  // static int defaultPort;
  int port;
  // server at specific port shared by all instances
  AsyncWebServer webServer;
  HTTPSServer secureServer;
  // each instances providing custom routes for the shared server
  // static std::vector<WebService<SERVER_PORT> *> *instances;
  // States @todo: support more states like running, stopped, failed..
  // static bool serverState;
  bool serverState = false; // service state
  WebServer();
  static WebServer &instance();
  void init();
  void start();
  void loop();
};

// SSLCert &cert;
// Declare some handler functions for the various URLs on the server
// The signature is always the same for those functions. They get two parameters,
// which are pointers to the request data (read request body, headers, ...) and
// to the response data (write response, set status code, ...)
void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);