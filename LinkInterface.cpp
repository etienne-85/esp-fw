#include <ApiModule.h>
#include <ArduinoJson.h>
#include <Events.h>
#include <LogStore.h>
#include <LinkInterface.h>
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
LinkInterface::LinkInterface(LinkInterfaceType interfaceType)
    : type(interfaceType){};

void LinkInterface::onPacket(std::string &msgContent) {
  Packet incomingMsg(type);
  incomingMsg.parse(msgContent);
  // LogStore::dbg(
  //     "[LinkInterface::onPacket] apiModule: " + apiCall.apiModule +
  //     ", call: " + apiCall.call + ", data: " + apiCall.data);
  if (true || !filterOutMessage(incomingMsg)) {
    // send ACK MSG
    if (incomingMsg.ack) {
      LinkInterface::sendAck(msgContent);
    }
    std::string outgoingReply = ApiModule::dispatchApiCall(incomingMsg);
    // optional sync reply
    if (outgoingReply.length() > 0) {
      // TODO send result
    }
  }
}

bool LinkInterface::filterOutMessage(Packet &incomingMsg) {

  std::string deviceId = System::cfgStore.configContent()["deviceId"];
  bool rejected =
      type == LinkInterfaceType::LORA && incomingMsg.target != deviceId;
  if (rejected) {
    LogStore::info("[LoraInterface::filterMessage] reject message from " +
                   incomingMsg.sender + " addressed to " + incomingMsg.target +
                   " received on " + deviceId);
  }
  return rejected;
}

void LinkInterface::sendAck(std::string &msgContent) {

  Packet ackMsg(type);
  ackMsg.parse(msgContent);
  ackMsg.target = ackMsg.sender; // swap sender and target
  ackMsg.api = "msgACK";
  ackMsg.data = "";
  ackMsg.ack = false;
  std::string ackMsgContent(ackMsg.serialize());
  LogStore::info("[LinkInterface::sendAck] #" + ackMsg.timestamp);
  notifyClient(ackMsgContent);
}