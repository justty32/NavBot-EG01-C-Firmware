#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "Config.h"

// 宣告全域變數的引用 (定義在 NavBot_C.ino)
extern float spd;
extern float L;
extern float R;
extern float H_goal;
extern float PIT_goal;
extern float ROL_goal;
extern float Y_goal;
extern int gait_mode;
extern bool key_stab;

class WebServerHandler {
public:
    WebServerHandler() : server(80) {}
    
    void init();
    
private:
    AsyncWebServer server;
    
    void handleCommand(AsyncWebServerRequest *request);
    void handleParamUpdate(AsyncWebServerRequest *request);
};

#endif
