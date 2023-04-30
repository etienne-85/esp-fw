#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <defaults.h>

/***
 * WebServer
 * extend this class to provide additional server routes
 * or services for a specific port
 */

class WebServer { //: public Service {
public:
    static std::map<int, WebServer *> *instances;
    static WebServer &defaultInstance;
    // static int defaultPort;
    int port;
    // server at specific port shared by all instances
    AsyncWebServer webServer;
    // each instances providing custom routes for the shared server
    // static std::vector<WebService<SERVER_PORT> *> *instances;
    // States @todo: support more states like running, stopped, failed..
    // static bool serverState;
    bool serverState = false; // service state
    WebServer();
    WebServer(int port);
    static WebServer *instance(int port);
    static WebServer &getDefault();
    void start();
};