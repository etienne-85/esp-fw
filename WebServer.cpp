#include <WebServer.h>
#include <defaults.h>
// #include <cstddef>
// #include <ostream>
#include "cert.h"
#include "private_key.h"
#include <SSLCert.hpp>
/**
 *   WebServer
 **/

// int WebServer::defaultPort = DEFAULT_PORT;

// Create an SSL certificate object from the files included above ...
SSLCert cert = SSLCert(example_crt_DER, example_crt_DER_len, example_key_DER,
                       example_key_DER_len);

// Create a server based on this certificate.
// The constructor has some optional parameters like the TCP port that should be
// used and the max client count. For simplicity, we use a fixed amount of
// clients that is bound to the max client count.

// Default port
WebServer::WebServer()
    : port(DEFAULT_PORT), webServer(DEFAULT_PORT),
      secureServer(&cert, SECURED_PORT, MAX_CLIENTS) {}

WebServer &WebServer::instance() {
  // check if service already exists, otherwise create one
  static WebServer singleton;
  return singleton;
}

void WebServer::init() {
  // Initialize the slots
  // for (int i = 0; i < MAX_CLIENTS; i++)
  //   activeClients[i] = nullptr;

  // For every resource available on the server, we need to create a
  // ResourceNode The ResourceNode links URL and HTTP method to a handler
  // function
  ResourceNode *nodeRoot = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode *node404 = new ResourceNode("", "GET", &handle404);

  // Add the root node to the server
  secureServer.registerNode(nodeRoot);

  // The websocket handler can be linked to the server by using a WebsocketNode:
  // (Note that the standard defines GET as the only allowed method here,
  // so you do not need to pass it explicitly)
  // WebsocketNode *chatNode = new WebsocketNode("/chat", &ChatHandler::create);

  // Adding the node to the server works in the same way as for all other nodes
  // secureServer.registerNode(chatNode);

  // Finally, add the 404 not found node to the server.
  // The path is ignored for the default node.
  secureServer.setDefaultNode(node404);
}

void WebServer::start() {
  if (!serverState) {
    std::cout << "[WebServer] Starting ..." << std::endl;
    // WebServer<SERVER_PORT>::server = new AsyncWebServer(SERVER_PORT);
    webServer.begin();
    std::cout << "[WebServer] Server running on port " << port << std::endl;

    secureServer.start();
    if (secureServer.isRunning()) {
      std::cout << "[WebServer] HTTPS Server running on port " << SECURED_PORT
                << std::endl;
      // Serial.println(WiFi.localIP());
    }
    serverState = true;
  }
}

void WebServer::loop() { secureServer.loop(); }

// HTTPSServer secureServer = HTTPSServer(&cert, 443, MAX_CLIENTS);

// The following HTML code will present the chat interface.
void handleRoot(HTTPRequest *req, HTTPResponse *res) {
  res->setHeader("Content-Type", "text/html");

  res->print(
      "<!DOCTYPE HTML>\n"
      "<html>\n"
      "   <head>\n"
      "   <title>ESP32 Test</title>\n"
      "</head>\n"
      "<body>\n"
      "<div>HTTPS Server: test page</div>\n"
      "</body>\n"
      "</html>\n");
}

void handle404(HTTPRequest *req, HTTPResponse *res) {
  // Discard request body, if we received any
  // We do this, as this is the default node and may also server POST/PUT
  // requests
  req->discardRequestBody();

  // Set the response status
  res->setStatusCode(404);
  res->setStatusText("Not Found");

  // Set content type of the response
  res->setHeader("Content-Type", "text/html");

  // Write a tiny HTTP page
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not "
               "found on this server.</p></body>");
  res->println("</html>");
}