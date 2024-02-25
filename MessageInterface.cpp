#include <ApiModule.h>
#include <ArduinoJson.h>
#include <Events.h>
#include <LogStore.h>
#include <MessageInterface.h>
#include <System.h>
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
MessageInterface::MessageInterface(MessageInterfaceType interfaceType)
    : type(interfaceType){};

void MessageInterface::onMessage(std::string &msgContent) {
  Msg incomingMsg(type);
  incomingMsg.parseContent(msgContent);
  // LogStore::dbg(
  //     "[MessageInterface::onMessage] apiModule: " + apiCall.apiModule +
  //     ", call: " + apiCall.call + ", data: " + apiCall.data);
  if (!filterOutMessage(incomingMsg)) {
    // send ACK MSG
    if (incomingMsg.acknowledgeDelivery) {
      notifyClient("ACK");
    }
    std::string outgoingReply = ApiModule::dispatchApiCall(incomingMsg);
    // optional sync reply
    if (outgoingReply.length() > 0) {
      // TODO send result
    }
  }
}

bool MessageInterface::filterOutMessage(Msg &incomingMsg) {

  std::string deviceId = System::cfgStore.configContent()["deviceId"];
  bool rejected =
      type == MessageInterfaceType::LORA && incomingMsg.clientKey != deviceId;
  if (rejected) {
    LogStore::info(
        "[LoraInterface::filterMessage] reject message addressed to " +
        incomingMsg.clientKey + " received on device " + deviceId);
  }
  return rejected;
}