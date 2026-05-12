#include "WebServerHandler.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebResources.h"

// 引用外部變數
extern float spd, L, R, H_goal, PIT_goal, ROL_goal, Y_goal;
extern int gait_mode;
extern bool key_stab;
extern int init_case;
extern ServoConfig servo_cfg;

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

    // 校準儲存
    server.on("/save_cal", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("h1")) servo_cfg.h_mid[0] = request->getParam("h1")->value().toFloat();
        if (request->hasParam("s1")) servo_cfg.s_mid[0] = request->getParam("s1")->value().toFloat();
        if (request->hasParam("h2")) servo_cfg.h_mid[1] = request->getParam("h2")->value().toFloat();
        if (request->hasParam("s2")) servo_cfg.s_mid[1] = request->getParam("s2")->value().toFloat();
        if (request->hasParam("h3")) servo_cfg.h_mid[2] = request->getParam("h3")->value().toFloat();
        if (request->hasParam("s3")) servo_cfg.s_mid[2] = request->getParam("s3")->value().toFloat();
        if (request->hasParam("h4")) servo_cfg.h_mid[3] = request->getParam("h4")->value().toFloat();
        if (request->hasParam("s4")) servo_cfg.s_mid[3] = request->getParam("s4")->value().toFloat();
        if (request->hasParam("ma")) servo_cfg.ma_case = request->getParam("ma")->value().toInt();
        if (request->hasParam("ip")) servo_cfg.in_pit = request->getParam("ip")->value().toFloat();
        if (request->hasParam("ir")) servo_cfg.in_rol = request->getParam("ir")->value().toFloat();
        request->send(200, "text/plain", "OK");
    });

    // 處理控制參數 (相容 Python 版不標準路徑)
    server.onNotFound([](AsyncWebServerRequest *request) {
        String url = request->url();

        if (url.indexOf("/spd=") != -1) spd = url.substring(5).toFloat();
        if (url.indexOf("/H=") != -1) H_goal = url.substring(3).toFloat();
        if (url.indexOf("/mode=") != -1) gait_mode = url.substring(6).toInt();
        if (url.indexOf("/stab=") != -1) key_stab = (url.substring(6).toInt() == 1);

        // L/R 轉向: URL 格式為 /L=x&R=y，需分別解析
        int l_pos = url.indexOf("/L=");
        if (l_pos != -1) {
            L = url.substring(l_pos + 3).toFloat();
            int r_pos = url.indexOf("&R=");
            if (r_pos != -1) R = url.substring(r_pos + 3).toFloat();
        } else {
            int r_pos = url.indexOf("/R=");
            if (r_pos != -1) R = url.substring(r_pos + 3).toFloat();
        }

        request->send(200, "text/plain", "OK");
    });

    server.begin();
}
