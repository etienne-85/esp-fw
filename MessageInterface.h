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
class MessageInterface {
public:
  virtual void notifyClient(std::string msg) = 0;
};

enum MessageInterfaceType { WS, LORA };

// to be handled by MessageInterface
struct OutgoingMessageEvent {
  MessageInterfaceType msgSrc; 
  std::string clientKey;
};

struct IncomingMessageEvent {
  MessageInterfaceType msgSrc; 
  std::string clientKey;
};