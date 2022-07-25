#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <WebServices.h>
#include <string>

/**
 *   OTA Update Service
 */

template <int SERVER_PORT> class OTAServiceWrapper : WebService<SERVER_PORT> {
public:
  OTAServiceWrapper() : WebService<SERVER_PORT>() {
    std::string route("/update");
    WebService<SERVER_PORT>::serviceTreePath +=
        "\n    |__ [OTAUpdater:" + std::to_string(SERVER_PORT) + route + "]";
  }

  void init() {
    std::cout << "[OTAUpdater] Starting OTA Update service" << std::endl;
    AsyncElegantOTA.begin(&WebService<SERVER_PORT>::server);
    // state = true;
    std::cout << "[OTAUpdater] instance running on port " << SERVER_PORT
              << std::endl;
  }

  std::string getServiceName() { return std::string("OTAUpdater"); }
};

/**
 * Static Server
 */

template <int SERVER_PORT> class StaticServer : WebService<SERVER_PORT> {
public:
  StaticServer() : WebService<SERVER_PORT>() {
    std::string route("/");
    WebService<SERVER_PORT>::serviceTreePath +=
        "\n    |__ [StaticServer:" + std::to_string(SERVER_PORT) + route + "]";
  }

  std::string getServiceName() { return std::string("StaticServer"); }

  void init() {
    // load static routes
    loadRoutes();
    // state = true;
    std::cout << "[StaticServer] listenning on port " << SERVER_PORT
              << std::endl;
  }

  void loadRoutes() {
    std::cout << "[WebService:" << SERVER_PORT
              << "] Loading static server routes" << std::endl;
    // Route for root / web page
    WebService<SERVER_PORT>::server.on(
        "/", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send(LittleFS, "/index.html", "text/html", false);
        });

    WebService<SERVER_PORT>::server.serveStatic("/", LittleFS, "/");
  }
};

// JSON stub string.
//
// Using a char[], as shown here, enables the "zero-copy" mode. This mode uses
// the minimal amount of memory because the JsonDocument stores pointers to
// the input buffer.
// If you use another type of input, ArduinoJson must copy the strings from
// the input to the JsonDocument, so you need to increase the capacity of the
// JsonDocument.
char jsonDataStub[] =
    "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

/**
 *  WS listener providing JSON message conversion
 */
template <int WS_PORT, const char *WS_PATH, int JSON_MSG_SIZE>
class JsonWebSocketListener : WebSocketListener<WS_PORT, WS_PATH> {

  std::string processMsg(std::string rawMsg) {
    // Allocate the JSON document
    //
    // Inside the brackets, 200 is the capacity of the memory pool in bytes.
    // Don't forget to change this value to match your JSON document.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    // StaticJsonDocument<200> doc;

    // StaticJsonDocument<N> allocates memory on the stack, it can be
    // replaced by DynamicJsonDocument which allocates in the heap.
    //
    // DynamicJsonDocument doc(200);
    StaticJsonDocument<JSON_MSG_SIZE> jsonMsg;
    // convert to a json object
    DeserializationError error = deserializeJson(jsonMsg, rawMsg);
    // Check parsing was successful.
    if (!error) {

      // Most of the time, you can rely on the implicit casts.
      // In other case, you can do doc["time"].as<long>();
      // const char *sensor = jsonMsg["sensor"];
      // long time = jsonMsg["time"];
      // double latitude = jsonMsg["data"][0];
      // double longitude = jsonMsg["data"][1];

      // // Print values.
      // Serial.println(sensor);
      // Serial.println(time);
      // Serial.println(latitude, 6);
      // Serial.println(longitude, 6);
      const char *action = jsonMsg["action"];
      std::cout << "[JsonWebSocketListener > processMsg] " << action << std::endl;

      std::string replyMsg("default reply");
      return replyMsg;
    }
  }
};

/**
 *  routing ws message to multiple sub services
 */
template <int SERVER_PORT, const char *SOCKET_PATH>
class WebSocketRouter : WebSocketService<SERVER_PORT, SOCKET_PATH> {
  // Message container shared by instances
  static StaticJsonDocument<200> jsonObj;

  std::string processMsg(std::string rawMsg) {
    // parse json string
    DeserializationError error = deserializeJson(jsonObj, rawMsg);
    std::string allReplies;
    // Test if parsing succeeds.
    if (!error) {
      // JsonObject jsonObj = jsonMsg.as<JsonObject>();

      // for (JsonPair kv : jsonObj)
      // {
      //     std::string serviceName(kv.key().c_str());
      //     JsonObject serviceData = kv.value().as<JsonObject>();

      // }
      // dispatch message to corresponding service
      std::string serviceName = jsonObj["service"];
      Serial.println(serviceName.c_str());
      for (auto service :
           *WebSocketService<SERVER_PORT, SOCKET_PATH>::instances) {
        Serial.println(service->name.c_str());
        // match service name
        if (service->name.compare(serviceName) == 0) {
          std::string log =
              "[WebSocket] message routed to service " + serviceName;
          Serial.println(log.c_str());
          //   service->process();
        }
      }

      allReplies.append("TODO");
    } else {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    return allReplies;
  }
};
