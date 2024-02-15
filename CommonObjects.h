#pragma once
#include <string>
/*
Object container to be put in message
Typical objects to be carried over messages are:
- API calls
- Foreign events
*/

// class MsgObj {
//   template <class T> static T &fromString(std::string msgObj);
//   template <class T> static T &fromMinifiedString(std::string msgObj);
//   // export foreign event content to send over MSG
//   virtual std::string toString() = 0;
//   virtual std::string toMinifiedString() = 0;
// };

// template <T>
// MsgObj::fromMsgObj(std::string msgObj){
//     return T::fromMsgObj(msgObj);
// }

// class ApiCall : public MsgObj {
class ApiCall {
public:
  std::string apiModule;
  std::string call;
  std::string data;

  // ApiCall(std::string apiModule, std::string call, std::string data);

  void fromMsg(std::string msg);
  void fromMinifiedString(std::string msg);
  std::string toMsg();
  std::string toMinifiedString();
};

/*

Event -- (export to EVTOBJ) --> MSG -- (parse from EVTOBJ) --> ForeignEvent
*/
// additional information about event
// events can be filtered based on context
typedef struct {
  // EventOrigin origin = EventOrigin::LOCAL;
  std::string source = "local"; // original source from where event comes from
  int timestamp = 0;
  int priority = 0;
} EventContext;

class Event {
public:
  std::string type;
  std::string data;
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