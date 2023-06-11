#include <WebServer.h>
#include <defaults.h>
// #include <cstddef>
// #include <ostream>

/**
 *   WebServer
 **/

// int WebServer::defaultPort = DEFAULT_PORT;

// Default port
WebServer::WebServer() : port(DEFAULT_PORT), webServer(DEFAULT_PORT) {}

// Custom port
WebServer::WebServer(int port)
    : port(port),
      webServer(port) { // Service("webserver: "+std::to_string(SERVER_PORT)) {
  // serviceTreePath += "|__ [WebService:" + std::to_string(SERVER_PORT) + "]";
  // instances->push_back(this);
  // index = instances->size() - 1;
  // std::cout << "[WebService:" << SERVER_PORT << "]"
  //           << " new service #" << index << std::endl;
}

WebServer *WebServer::instance(int port) {
  // check if service already exists, otherwise create one
  if (auto search = WebServer::instances->find(port);
      search != WebServer::instances->end()) {
    std::cout << "existing service found" << std::endl;
  } else {
    std::cout << "no existing webserver found on port " << port << "=> creating"
              << std::endl;
    (*WebServer::instances)[port] = new WebServer(port);
  }
  return (*WebServer::instances)[port];
}

WebServer &WebServer::getDefault() {
  // check if service already exists, otherwise create one
  static WebServer defaultInstance;
  return defaultInstance;
}

void WebServer::start() {
  if (!serverState) {
    std::cout << "[WebServer:" << port << "] First instance : starting Server"
              << std::endl;
    // WebServer<SERVER_PORT>::server = new AsyncWebServer(SERVER_PORT);
    webServer.begin();
    serverState = true;
    std::cout << "[WebServer:" << port << "] Server running on port " << port
              << std::endl;
  }
}