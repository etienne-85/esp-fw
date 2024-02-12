#pragma once
#include <MessageInterface.h>
#include <map>
#include <queue>
#include <string>
#include <vector>

/*
EVT/MSG

  [LocalEventQueue] -- evt --> [EventHandler<forwarder>] -->
[RemoteMessageInterface] --> msg [RemoteMessageInterface] --> msg -->
[MessageListener<NotMsg>] --> NotEvt

Events are queud in EventQueue by EventQueue and dequeud when sent over
specific interface to specific client When new event is pushed in the queue, the
QueueProcessor is informed, so it can schedule event notification The
QueueProcessor is able to know which remote client has subscribed to which event
and relay information. Once all client subscribers have been notified event can
be removed from main Queue, and moved to SentQueue or RetryQueue if failed to be
transmitted  which will reschedule event notification at later moment.
 EventNotification can require receipt confirmation.

std::string examples:
- logEvt
- pinEvt

 EventQueue
 LogEventQueue
 PinEventQueue

 Any class
willing to receive events will have to inherit EventHandler class. Depending on
EventType, it will be LogEventListener, PinEventListener For remote listeners:
LogEventRemoteListener, PinEventRemoteListener Any remote client subscribing for
an event type will instantiate corresponding class
- LogEventRemoteListener:LogEventListener:EventHandler
- PinEventRemoteListener:PinEventListener:EventHandler
 Usecase examples:
 2 local process FsLog and SerialLog subscribes to LogEvt => 2 LogEventListener
 1 remote client subscribes to LogEvt => 1 LogEventRemoteListener
 1 local process PirAlarm subscribe to PinEvt => 1 PinEventListener
 1 remote process subscribe to PinEvt => 1 PinEventRemoteListener
 SubTotal:
- 3 LogEventListener (2 LogEventListener + 1 LogEventRemoteListener)
- 2 PinEventListener (1 PinEventListener + 1 PinEventRemoteListener)
Total: 5 EventListeners (3 LogEventListener + 2 PinEventListener)

SUMMARY:
EventType:
- LogEventType:EventType,
- PinEventType:EventType
EventQueue:
- LogEventQueue: EventQueue,
- PinEventQueue: EventQueue
EventQueue:
- LogEventPublisher:EventQueue,
- PinEventPublisher:EventQueue
EventHandler:
- LogEventListener:EventHandler
- PinEventListener:EventHandler

SIMPLIFICATION
EventType: LogEventType, PinEventType
EventQueue storing EventType
EventQueue<EventType>: publish events (LogEventType or PinEventType) to
EventQueue EventHandler<EventType>: subscribes to events notfications
RemoteEventListener<EventType>: additional logic to notify clients over an
interface MessageNotificationHandler: handling remote client subscription,
instancing RemoteEventListener

*/

// local: triggered from within system
// remote/external/injected: received from external source (through messages)
// and injected in queue
enum EventOrigin { LOCAL, REMOTE };

typedef struct {
  EventOrigin origin = EventOrigin::LOCAL;
  int timestamp = 0;
  int priority = 0;
} EventContext;

typedef struct {
  std::string type;
  EventContext context;
  std::string data;
} Event;

enum EventType { LOG, PIN, TST, MSGNOT, MSGFWD, MSGREP };
extern std::map<EventType, std::string> EventTypeMap;

class EventQueue {
  static std::queue<Event> events;

public:
  static void pushEvent(Event evt, bool bypassEvtQueue = false);
  static void watchEvents();
};

/*
 * Any class inheriting EventHandler and overriding onEvent
 * receives event notifications
 */
class EventHandler {
public:
  static std::vector<EventHandler *> subscribers;
  std::string evtType;

protected:
  EventHandler(std::string evt);

public:
  static bool dispatchEvt(Event evt);

  // static void notify(int pinId, int pinVal);
  virtual void onEvent(std::string eventData);
};

class RemoteEventListener : public EventHandler {};