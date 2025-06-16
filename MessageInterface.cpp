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
  if (true || !filterOutMessage(incomingMsg)) {
    // send ACK MSG
    if (incomingMsg.acknowledgeDelivery) {
      MessageInterface::sendAck(msgContent);
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
      type == MessageInterfaceType::LORA && incomingMsg.target != deviceId;
  if (rejected) {
    LogStore::info("[LoraInterface::filterMessage] reject message from " +
                   incomingMsg.sender + " addressed to " + incomingMsg.target +
                   " received on " + deviceId);
  }
  return rejected;
}

void MessageInterface::sendAck(std::string &msgContent) {

  Msg ackMsg(type);
  ackMsg.parseContent(msgContent);
  ackMsg.target = ackMsg.sender; // swap sender and target
  ackMsg.apiCall = "msgACK";
  ackMsg.objContent = "";
  ackMsg.acknowledgeDelivery = false;
  std::string ackMsgContent(ackMsg.serialize());
  LogStore::info("[MessageInterface::sendAck] #" + ackMsg.msgId);
  notifyClient(ackMsgContent);
}