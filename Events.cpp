#include <Events.h>
#include <LogStore.h>
#include <LoraInterface.h>
#include <WsInterface.h>

// std::map<std::string, EventType> EventTypeMap{{"LOG", LOG},
//                                               {"PIN", PIN},
//                                               {"MSGNOT", MSGNOT},
//                                               {"MSGFWD", MSGFWD},
//                                               {"MSGREP", MSGREP}};
std::map<EventType, std::string> EventTypeMap{
    {LOG, "LOG"}, {PIN, "PIN"}, {EXT, "EXT"}};

/*
 * EventTrigger
 */

std::queue<Event> EventQueue::local;

void EventQueue::pushEvent(Event evt, bool bypassEvtQueue) {
  // will directly call evtDispatch synchronously
  if (bypassEvtQueue) {
    EventHandler::dispatchEvt(evt);
  } else {
    EventQueue::local.push(evt);
  }
};

void EventQueue::watchEvents() {
  if (EventQueue::local.size() > 0) {
    Event evt = EventQueue::local.front();
    bool dispatched = EventHandler::dispatchEvt(evt);
    if (!dispatched) {
      LogStore::dbg("[EventQueue::watchEvents] undispatched event " + evt.type +
                    " => put in "
                    "waiting queue ");
    }
    EventQueue::local.pop();
  }
};

/*
 * EventHandler
 */

std::vector<EventHandler *> EventHandler::subscribers;

EventHandler::EventHandler(std::string evtType) : evtType(evtType) {
  subscribers.push_back(this);
  //   LogStore::dbg("[EventHandler::constructor] instances count " +
  //                 std::to_string(subscribers.size()));
};

bool EventHandler::dispatchEvt(Event evt) {
  //   LogStore::dbg("[EventHandler::dispatchEvt] " + eventData, true);
  bool dispatched = false;
  for (EventHandler *instance : EventHandler::subscribers) {
    if (instance->evtType == "*" || evt.type == instance->evtType) {
      instance->onEvent(evt);
      dispatched = true;
    }
  }
  return dispatched;
};

void EventHandler::onEvent(Event evt) {
  LogStore::dbg("[EventHandler::onEvent] ");
};

// WsEventRemoteListener *WsEventRemoteListener::getInstance(EventType evtType,
//                                                           int clientId) {
//   LogStore::info("[MessageNotificationHandler::subscribe] client " + clientId
//   +
//                  " will now receive notifications for event " + evtType);
// };

// LoraEventRemoteListener *
// LoraEventRemoteListener::getInstance(EventType evtType, std::string deviceId)
// {
//   LogStore::info("[MessageNotificationHandler::subscribe] device " + deviceId
//   +
//                  " will now receive notifications for event " + evtType);
// };