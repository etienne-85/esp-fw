#pragma once
#include <Arduino.h>
#include <string>
// #include <MessageInterface.h>
// use forward declaration instead of former include
// enum MessageInterfaceType; // doesn't compile
enum MessageInterfaceType { WS, LORA };

/*
Typical objects to be carried over messages are:
- API calls
- Foreign events
*/

class Msg {
public:
  std::string msgId = std::to_string(millis()); // device's local ms
  std::string apiModule;
  std::string apiCall;
  std::string objContent; // optional object content to be processed by API
  std::string sender;
  std::string target;
  MessageInterfaceType interface; // source interface of incoming msg or target
                                  // interface for outgoing msg
  bool acknowledgeDelivery = false; // insure message delivery
  int retryCount;                   // number of attempts sending message
  Msg(MessageInterfaceType msgInterfaceType);
  void parseContent(std::string msgContent);
  std::string serialize();
  // TODO minification/encryption
  void decode(std::string msg);
  std::string encode();
  void send(); // send through specified interface
};

/*

Event -- (export to EVTOBJ) --> MSG -- (parse from EVTOBJ) --> ForeignEvent
*/
// additional information about event
// events can be filtered based on context
typedef struct {
  // EventOrigin origin = EventOrigin::LOCAL;
  std::string source = "local"; // original source from where event comes from
  int timestamp = millis();
  int priority = 0;
} EventContext;

class Event {
public:
  std::string type;
  std::string content;
  EventContext context;
  // serialize event content to send over MSG
  std::string toObjContent();
};
/*
apiCall: "EvtMsgNot:onForeignEvt"
orig:
*/
class ForeignEvent : public Event {
public:
  // std::string source;
  ForeignEvent();
  ForeignEvent(Event evt, std::string source);
  void fromObjContent(std::string objContent);
};

/*
 * depending on pin type some fields are applicable or not
 */
class PinConf {
public:
  int pinNb;
  int type; // PIN_TYPE
  int chan;
  int freq = 10000;
  int res = 8;

  void fromObjContent(std::string msg);
  std::string toObjContent();
};