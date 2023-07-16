#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <string>

/**
 *   OTA Update Service
 */

class OTAServiceWrapper {
public:

  void init() {
    std::string route("/update");
    std::cout << "[OTAUpdater] Starting OTA Update service" << std::endl;
    AsyncElegantOTA.begin(&WebServer::instance().webServer);
    // state = true;
    std::cout << "[OTAUpdater] available at " << route << std::endl;
  }

  std::string getIdentifier() { return std::string("OTAUpdater"); }
};

/**
 * Static Server
 */

class StaticServer {
public:
  StaticServer() {
    std::string route("/");
    // WebService<SERVER_PORT>::serviceTreePath +=
    //     "\n    |__ [StaticServer:" + std::to_string(SERVER_PORT) + route + "]";
  }

  std::string getIdentifier() { return std::string("StaticServer"); }

  void init() {
    // load static routes
    loadRoutes();
    // state = true;
    std::cout << "[StaticServer] available " << std::endl;
  }

  void loadRoutes() {
    std::cout << "[StaticServer] Loading custom routes" << std::endl;
    // Route for root / web page
    WebServer::instance().webServer.on(
        "/", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send(LittleFS, "/index.html", "text/html", false);
        });

    WebServer::instance().webServer.serveStatic("/", LittleFS, "/");
  }
};
