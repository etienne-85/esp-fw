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
*/
enum MessageInterfaceType { WS, LORA };

class MessageInterface {
protected:
  MessageInterfaceType type;
  virtual std::string onMessage(std::string incomingMsg);
  std::string injectEvent(std::string incomingMsg);

public:
  virtual void notifyClient(std::string msg) = 0;
};