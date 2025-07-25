#include <ApiModule.h>
#include <ArduinoJson.h>
#include <LogStore.h>
#include <System.h>

ApiModule::ApiModule(std::string apiModule) {
  LogStore::info("[ApiModule::constructor] register API module: " + apiModule);
  ApiModule::registeredApiModules.insert({apiModule, this});
}

// STATIC DEF
std::map<std::string, ApiModule *> ApiModule::registeredApiModules;

std::string ApiModule::dispatchApiCall(Packet &msg) {
  // TODO
  // std::string msgSrc = type; // interface type (LORA,WS)
  // std::string msgCtx = msgRoot["ctx"];
  // std::string clientKey = msgCtx["clientKey"];
  // int msgTime = msgRoot["time"];
  // if (msgTime) {
  //   System::time.sync(msgTime);
  // }
  // std::string msgMode = msgRoot["mode"]; // sync or async
  // bool bypassEvtQueue(msgMode == "sync");
  // EventQueue::pushEvent(event, bypassEvtQueue);
  // find corresponding sub service
  auto apiModule = ApiModule::registeredApiModules.find(msg.api);
  if (apiModule != ApiModule::registeredApiModules.end()) {
    LogStore::dbg("[ApiModule::dispatchApiCall] API module: " + msg.api);
    std::string dataOut = apiModule->second->onApiCall(msg);
    //     // default empty reply
    //     std::string outgoingMsg("");
    //     bool expectedReply = timestamp > 0 || dataOut.length() > 0;
    //     // fill with data from service if any
    //     if (expectedReply) {
    //       JsonDocument replyData;
    //       deserializeJson(replyData, dataOut);
    //       // Build the JSON reply including service output
    //       // and additional fields (msgType, timestamp)
    //       JsonDocument replyRoot;
    //       replyRoot["handle"] = handle;
    //       if (timestamp > 0) {
    //         LogStore::info("[ApiModule::extractMsg] timestamp
    //         provided " +
    //                        std::to_string(timestamp) +
    //                        " => expected reply confirmation");
    //         replyRoot["timestamp"] = timestamp;
    //       }
    //       if (dataOut.length() > 0) {
    //         replyRoot["data"] = replyData;
    //       }
    //       serializeJsonPretty(replyRoot, outgoingMsg);
    //     }
    //     return outgoingMsg;
  } else {
    LogStore::info("[ApiModule::dispatchApiCall] unregistered API module: " +
                   msg.api);
  }
  return "";
}

// std::string ApiModule::dispatchMsg(std::string incomingMsg,
//                                            std::string clientKey) {
//   JsonDocument msgRoot;
//   // convert to a json object
//   DeserializationError error = deserializeJson(msgRoot, incomingMsg);

//   // root level props common to all services
//   std::string handle = msgRoot["handle"];
//   int timestamp = msgRoot["timestamp"];
//   if (timestamp) {
//     System::time.sync(timestamp);
//   }
//   // find corresponding sub service
//   auto msgHandler = ApiModule::registeredApiModules.find(handle);
//   if (msgHandler != ApiModule::registeredApiModules.end()) {
//     // LogStore::info("[ApiModule::dispatchMsg] dispatch to message
//     // handler " +msgType); forward message to subservice handler
//     std::string dataOut = msgHandler->second->onCall(incomingMsg,
//     clientKey);
//     // default empty reply
//     std::string outgoingMsg("");
//     bool expectedReply = timestamp > 0 || dataOut.length() > 0;
//     // fill with data from service if any
//     if (expectedReply) {
//       JsonDocument replyData;
//       deserializeJson(replyData, dataOut);
//       // Build the JSON reply including service output
//       // and additional fields (msgType, timestamp)
//       JsonDocument replyRoot;
//       replyRoot["handle"] = handle;
//       if (timestamp > 0) {
//         LogStore::info("[ApiModule::extractMsg] timestamp provided
//         " +
//                        std::to_string(timestamp) +
//                        " => expected reply confirmation");
//         replyRoot["timestamp"] = timestamp;
//       }
//       if (dataOut.length() > 0) {
//         replyRoot["data"] = replyData;
//       }
//       serializeJsonPretty(replyRoot, outgoingMsg);
//     }
//     return outgoingMsg;
//   } else {
//     LogStore::info("[ApiModule::dispatchMsg] no registered message
//     "
//                    "handler matching " +
//                    handle);
//   }
//   return "";
// }