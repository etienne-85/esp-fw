#pragma once
#include <MessageInterface.h>
#include <string>
#include <vector>

/*
 #1
 -- msg --> [MSGINTERFACE] -- MsgInEvt --> [IncomingMessageHandler]
 -- evt --> [EVTFORWARDER] -- MsgOutEvt --> [MSGINTERFACE]
 #2
 -- msg --> [MSGINTERFACE] --> [MSGLISTENER] -->


EVT/MSG

  [LocalEventQueue] -- evt --> [EventHandler<forwarder>] -->
[RemoteMessageInterface] --> msg [RemoteMessageInterface] --> msg -->
[MessageListener<NotMsg>] --> NotEvt
*/

enum EventType { LogEvt, PinEvt, NotEvt, IncomingMsgEvt, OutgoingMsgEvt };

/**
To be inherited from any class pushing events
*/
class EventTrigger {
  static std::vector<EventTrigger *> publishers;

public:
  static void pushEvent(std::string eventData, EventType eventType);
};

/*
 * Any class inheriting EventHandler and overriding onEvent
 * receives event notifications
 */
class EventHandler {
public:
  static std::vector<EventHandler *> subscribers;
  EventType evtType;

protected:
  EventHandler(EventType evt);

public:
  static void dispatchEvt(std::string eventData, EventType eventType);

  // static void notify(int pinId, int pinVal);
  virtual void onEvent(std::string eventData);
};

/*
bridge between publishers and listeners
*/
class EventQueue {
  void enqueue();
  void dequeue();
};

class RemoteEventListener : public EventHandler {};