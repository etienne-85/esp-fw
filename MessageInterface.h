#pragma once
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
enum MessageInterfaceType { WS, LORA };

class MessageInterface {
protected:
  MessageInterfaceType type;
  virtual std::string onMessage(std::string incomingMsg);

public:
  virtual void notifyClient(std::string msg) = 0;
};