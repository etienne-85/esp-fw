#include <ApiModule.h>
#include <ArduinoJson.h>
#include <Events.h>
#include <LogStore.h>
#include <MessageInterface.h>
/*
* msg = {
    type: 'api',
    api: {
      module: "TestModule",
      input: {
        call: "sandbox",
        args: ""
      }
    }
}

reqInput = {
  call: "sandbox",
  args: ""
}

apiRequest = {
  module: "TestModule",
  input: reqInput
}

msg = {
  type: "api",
  data: apiRequest
}
*/
std::string MessageInterface::onMessage(std::string incomingMsg) {
  std::string res("");
  std::string interfaceType = "<type>"; // LORA or WS
  LogStore::dbg("[MessageInterface::onMessage] received message on interface " +
                interfaceType + ": " + incomingMsg);
  ApiCall apiCall;
  apiCall.fromMsg(incomingMsg);
  // LogStore::dbg(
  //     "[MessageInterface::onMessage] apiModule: " + apiCall.apiModule +
  //     ", call: " + apiCall.call + ", data: " + apiCall.data);
  res = ApiModule::dispatchApiCall(apiCall);
  // root msg level props
  // JsonObject req = originalMsg["content"];
  // std::string apiRequest;
  // serializeJson(req, apiRequest);

  // if (msgType == "apiCall") {
  //   // TODO msgContext
  //   // JsonDocument msgContext;
  //   // msgContext["clientKey"] = clientKey;
  //   // msgContext["interfaceType"] = interfaceType;
  //   // msgRoot["context"] = msgContext;
  //   // std::string patchedMsg(incomingMsg);
  //   std::string apiRequest;
  //   serializeJson(req, apiRequest);
  //   res = ApiModule::dispatchApiReq(apiRequest);
  // } else if (msgType == "evtInject") {
  //   injectEvent(incomingMsg);
  // }
  return res;
}