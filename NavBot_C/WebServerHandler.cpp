#include "WebServerHandler.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebResources.h"

// 引用外部變數
extern float spd, L, R, H_goal, PIT_goal, ROL_goal, Y_goal;
extern int gait_mode;
extern bool key_stab;
extern int init_case;

AsyncWebServer server(80);

void WebServerHandler::init() {
    // 1. 建立 AP
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ssid[20];
    sprintf(ssid, "EG01-%02X%02X%02X", mac[3], mac[4], mac[5]);
    
    WiFi.softAP(ssid);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // 2. 路由設定 - 這裡改為使用 PROGMEM 字串
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", INDEX_HTML);
    });

    server.on("/cal", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", CAL_HTML);
    });

    // 處理控制參數 (相容 Python 版不標準路徑)
    server.onNotFound([](AsyncWebServerRequest *request) {
        String url = request->url();
        
        if (url.indexOf("/spd=") != -1) spd = url.substring(5).toFloat();
        if (url.indexOf("/L=") != -1) L = url.substring(3).toFloat();
        if (url.indexOf("/R=") != -1) R = url.substring(3).toFloat();
        if (url.indexOf("/H=") != -1) H_goal = url.substring(3).toFloat();
        if (url.indexOf("/mode=") != -1) gait_mode = url.substring(6).toInt();
        if (url.indexOf("/stab=") != -1) key_stab = (url.substring(6).toInt() == 1);
        
        request->send(200, "text/plain", "OK");
    });

    server.begin();
}
