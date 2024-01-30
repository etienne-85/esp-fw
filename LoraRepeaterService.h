#include <ArduinoJson.h>
#include <LogStore.h>
#include <string>

#define SERVICE_NAME "loraRepeater"
/*
ECHO, REPEATER, GATEWAY
register as remote service (both available on WS and LORA interface) 
Its sole purpose is to echo message on LORA interface 
or proxy message received on WS over LORA interface


Recursive repeating example
Browser tells Device#1 to forward wrapped message over LORA (Gateway)
Device#1 sends wrapped message over LORA telling Device#2 to ECHO wrapped
message over same interface
Device#2 repeat wrapped message over LORA


{   // repeat level 0 sent from browser over WS
    svcId: "repeat"
    wrap: {  // repeat level 1 (forwarded by device#1)
        dst: "device#2",    // prevent device#1 from receiving its own message
        svcId: "repeat",
        wrap: { // repeat level 2 (echoed by device#2)
            dst: "device#1", // prevent device#2 from receiving its own message
            svcId: "log",        // command or service to call
            svcIn: "input service data"
        }
    }
}*/

/*
 *   Supporting recursive repeating
 */
class LoraRepeaterService : public RemoteServiceListener {
public:
  static LoraRepeaterService &instance();

private:
  LoraRepeaterService();

public:
  std::string onServiceCall(std::string incomingMsg, int clientId = -1);
};

/**************************
 *** STATIC DEFINITIONS ***
 **************************/

LoraRepeaterService &LoraRepeaterService::instance() {
  static LoraRepeaterService singleton;
  return singleton;
}

LoraRepeaterService::LoraRepeaterService()
    : RemoteServiceListener(SERVICE_NAME) {}

std::string LoraRepeaterService::onServiceCall(std::string incomingMsg, int clientId) {
  LogStore::info("[LoraRepeaterService::onServiceCall] ");
  JsonDocument root;
  // convert to a json object
  DeserializationError error = deserializeJson(root, incomingMsg);

  // extract wrapMsg and send over LORA
  std::string wrapMsg = root["wrap"]; // null if not filled
  LoraRemoteInterface::instance().send(wrapMsg);
  // default empty reply
  return "";
}

#undef SERVICE_NAME
