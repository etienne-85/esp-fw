/*
MessageNotificationHandler is handling client request for event notification.
For each new client subscribing to specific event it will create corresponding
instance of MessageEventNotifier
(e.g. LogEventRemoteListener for LOGS or
PinEventRemoteListener for PINS)

- LogEventRemoteListener:LogEventListener:EventHandler
- PinEventRemoteListener:PinEventListener:EventHandler
MessageNotificationHandler

2 remote mechanism
- POLLING: external client performs action:
- NOTIFICATION: device push event notification to registered clients

POLLING mode usecase examples:
- LOG/PIN data pull,
- PIN control,
- FS operation (change setting in file)

NOTIFICATION mode usecase examples:
- subscribe log notification
- subscribe pin state change
*/
// HPP

#pragma once

#include <ApiModule.h>
#include <ArduinoJson.h>
#include <map>
#include <string>
// CPP
#define API_MODULE "MsgNotif"
#include <CommonObjects.h>
#include <HTTPSServer.hpp>
#include <LogStore.h>
#include <MessageInterface.h>
#include <WsInterface.h>

/**
## SIMPLIFICATION ##
Unique singleton for:
- notifying remote clients of events (catching event & sending msg notification)
- be notified of external events: handling API call to inject event when msg
notification is received

Event notification: by default sending all events to all connected client on WS
interface. When enabled, sending to all devices through LORA

Events received from outside through message notification are injected locally
and will in turn be automatically forwarded to any connected client on WS
interface

               +-----------+     +-----------------------------+
      MSG  --> |   LORA    | --> |  EventMessageNotifications  |  (api call from
MSG) | Interface |     |  ::onForeignEvent      |
               +-----------+     +-----------------------------+
                                              |
                                              | push evt<ext>
                                              v
                                        +-----------+
                                        | EVT QUEUE |
                                        +-----------+
                                              | catch evt<ext>
                                              v
+--------+     +-----------+     +---------------------------+
| CLIENT | <-- |    WS     | <-- |  EventMessageNotifications|  (onEvent
callback)
+--------+     | Interface |     |      ::onEvent            |
               +-----------+     +---------------------------+
*/

class EventMessageNotifications : public ApiModule, public EventHandler {
  bool autoNotification = true;  // only for WS
  bool loraNotification = false; // off by default

public:
  static EventMessageNotifications &instance();

private:
  EventMessageNotifications();

public:
  // catching events from queue, sending notification message
  void onEvent(Event evt);
  std::string onApiCall(ApiCall &apiCall);
  // receiving message notification, pushing event in queue
  void onForeignEvent(std::string evtContent);

  // void notify();
  // std::string onEventSubscribe(std::string eventType, std::string clientKey);
  // subscribe to event from another device through LORA
  // this device will be a relay between remote device and client subscriber
  // std::string onEventForwardSubscribe(std::string eventType,
  //                                     std::string clientKey);
};

EventMessageNotifications::EventMessageNotifications()
    : ApiModule(API_MODULE), EventHandler("*") {
  LogStore::info("[EventMessageNotifications] Event handler + API service: " +
                 std::string(API_MODULE));
}

// STATIC
EventMessageNotifications &EventMessageNotifications::instance() {
  static EventMessageNotifications singleton;
  return singleton;
}

// Catch events, wrap event in API call and send notification message
void EventMessageNotifications::onEvent(Event evt) {
  // LogStore::dbg("[EventMessageNotifications::onEvent] " + clientKey +
  //                   " received event " + eventData,
  //               true);
  LogStore::dbg("[EventMessageNotifications::onEvent] ");
  std::string deviceId = System::cfgStore.configContent()["deviceId"];
  std::string eventSrc =
      evt.context.source == "local" ? deviceId : evt.context.source;
  ForeignEvent fEvt(evt, deviceId);
  std::string evtContent = fEvt.toObjContent();
  LogStore::dbg("[EventMessageNotifications::onEvent] evtContent: " +
                evtContent);
  ApiCall apiCall;
  apiCall.apiModule = API_MODULE;
  apiCall.call = "onForeignEvent";
  apiCall.data = evtContent;
  std::string apiCallMsg = apiCall.toMsg();
  // notify client
  // WsInterface::clientInstance(clientKey)->notifyClient(eventNotifMsg);
  WsInterface::notifyAll(apiCallMsg);
};

// void EventMessageNotifications::extractMsg(std::string rawMsg) {
std::string EventMessageNotifications::onApiCall(ApiCall &apiCall) {
  LogStore::info("[EventMessageNotifications::onApiCall] ");

  // module, submodule, command
  // std::string apiCall = apiCallData["call"];
  // std::string apiCommand = apiInput["cmd"];
  // if (apiCall == "subscribe") {
  //   JsonObject apiInput = apiCallData["input"];
  //   std::string evtType = apiInput["evtType"];
  //   std::string clientKey = apiInput["clientKey"];
  //   return onEventSubscribe(evtType, clientKey);
  // } else
  if (apiCall.call == "onForeignEvent") {
    onForeignEvent(apiCall.data);
  }
  return ("");
}

// Foreign event received from message notification => push local event
void EventMessageNotifications::onForeignEvent(std::string evtContent) {
  ForeignEvent foreignEvt;
  foreignEvt.fromObjContent(evtContent);
  Event &evt = (Event &)foreignEvt;
  EventQueue::pushEvent(evt, false);
}

// ################
// ## DEPRECATED ##
// ################

/*Singleton managing event subscription by assigning dedicated
MessageEventNotifier instance per event type and client id
- cli#1 subscribes evtA => create instance
- cli#1 subscribes evtB => create instance
- cli#2 subscribes evtA => create instance
- cli#2 subscribes evtB => create instance
TOTAL: 4 MessageEventNotifier instances*/

// MessageEventNotifier *MessageEventNotifier::getInstance(std::string evtType,
//                                                         std::string
//                                                         clientKey, bool
//                                                         autoInstanciate) {

//   MessageEventNotifier *existing = nullptr;
//   for (MessageEventNotifier *instance :
//        MessageEventNotifier::clientSubscribers) {
//     if (instance->clientKey == clientKey)
//       existing = instance;
//   }
//   return existing != nullptr ? existing
//                              : new MessageEventNotifier(
//                                    EventTypeMap[EventType::LOG], clientKey);
// };

/*
 * Allowing client to be notified of any events without having to poll regularly
 TODO: support min delay config between logs notification
 TODO#2: confirm subscription
 svcOut: {
  event: "subscribe"
  status: "confirmed"
 }
 */
// std::string EventMessageNotifications::onEventSubscribe(std::string evtType,
//                                                         std::string
//                                                         clientKey) {
//   if (clientKey != "") {
//     LogStore::info("[EventMessageNotifications::subscribe] subscription "
//                    "received from client " +
//                    clientKey);
//     // MessageInterface *clientInterfaceInstance =
//     //     (MessageInterface *)WsInterface::clientInstance(clientKey);
//     // MessageEventNotifier *instance =
//     //     MessageEventNotifier::getInstance(evtType, clientKey);
//     // instance->msgInterface = clientInterfaceInstance;
//   } else {
//     LogStore::info("[EventMessageNotifications::subscribe] missing clientKey
//     ");
//     // TODO reply with error status
//   }
//   return "";
// }

#undef API_MODULE
