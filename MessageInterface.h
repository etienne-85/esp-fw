#pragma once
#include <CommonObj.h>
#include <string>

/*
Interface between ESP and outside world enabling ESP to talk
with remote clients
                    +---------------+
outgoingMsg <-----  |   Message     | <-----
incomingMsg ----->  |   Interface   | ----
                    +---------------+     |
                                          v
Message is object carryier
  MSG {
    call: module:call,
    data: obj           // ForeignEvt, ..
  }
*/
// enum MessageInterfaceType { WS, LORA };

class MessageInterface {
  // awaiting delivery
  // for WS, messages will remain in pending queue until client connects
  // static std::queue<Message> pending;
  // retry at later time => push again in pending queue
  //  message that didn't get receipt confirmation
  // static std::queue<Message> undelivered;
public:
  static void sendMsg(Msg msg);
  MessageInterfaceType type;

protected:
  MessageInterface(MessageInterfaceType interfaceType);
  void onMessage(std::string &msgContent);
  bool filterOutMessage(Msg &incomingMsg);

public:
  virtual void notifyClient(std::string msg) = 0;
};