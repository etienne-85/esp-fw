#include <ArduinoJson.h>
#include <string>

#define JSON_SIZE DEFAULT_JSON_SIZE

/*
* Telling if GPIO Pin is of PWM type from its config
*/
bool isPwm (std::string pinData){
    StaticJsonDocument<JSON_SIZE> root;
    // unpack json string
    deserializeJson(root, pinData);
    // check pin type
    int type = root["type"];      
    return type==1;
}