#include <ESPAsyncWebServer.h>
#include <map>

/***
 * Each service inheriting this class will be notified of incoming ws message
 * modes:
 * - synchroneous
 * - asynchroneous listening TODO
 *
 * Cascading instanciation
 * service -> WebSocketListener @ /service -> WebServer @ :port
 *
 */

class WebSocketListener {
public:
  AsyncWebSocket webSocket; // initialised in constructor
                            // instances mapping: one recipient per route key
  static std::map<std::string, WebSocketListener *> *recipients;
protected:
  // static int totalMessageCount = 0;
  int messageCount = 0;  // messages count received for current route
  // int hearbeatsCount=0; 
  // int aliveSignalRate;
  std::string rawMessage = ""; // last message received for current route
  // instanciation must be made through instance method
  WebSocketListener(std::string routeKey);

public:
  // active async listen to a specific WS service
  // static void asyncListenForwardLoop();

  // return existing instance for given route
  static WebSocketListener *instance(std::string key);

  static WebSocketListener *findRecipient(std::string route);

  // void listen(int wsPort, const char *wsPath);

  static void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client,
                           AwsEventType type, void *arg, uint8_t *data,
                           size_t len);

  // static void dispatchMsg(std::string msg, std::string dest);
  // dispatch message to corresponding recipient depending on route
  static void dispatchMsg(void *arg, uint8_t *data, size_t len, int clientId,
                          std::string route);

  // message handling implemented by recipient
  virtual void unpackMsg(std::string rawMsg);

  virtual void loop();

  // virtual std::string reply(std::string mesg);
};