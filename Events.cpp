#include <Events.h>
#include <LogStore.h>
#include <LoraInterface.h>
#include <WsInterface.h>

/*
 * EventTrigger
 */

std::vector<EventTrigger *> EventTrigger::publishers;

void EventTrigger::pushEvent(std::string eventData, EventType eventType) {
  // LogStore::info("[EventTrigger::pushEvent] eventType: " +
  //                    std::to_string(eventType) + ", eventData: " +
  //                    eventData,
  //                true);
  EventHandler::dispatchEvt(eventData, eventType);
};

/*
 * EventHandler
 */

std::vector<EventHandler *> EventHandler::subscribers;

EventHandler::EventHandler(EventType evtType) : evtType(evtType) {
  subscribers.push_back(this);
  //   LogStore::dbg("[EventHandler::constructor] instances count " +
  //                 std::to_string(subscribers.size()));
};

void EventHandler::dispatchEvt(std::string eventData, EventType eventType) {
  //   LogStore::dbg("[EventHandler::dispatchEvt] " + eventData, true);
  for (EventHandler *instance : EventHandler::subscribers) {
    instance->onEvent(eventData);
  }
};

void EventHandler::onEvent(std::string eventData) {
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