#include "DNSServer.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

const char* ssid = "scoreboard";
const char* password = "12345678";

DNSServer dnsServer;
AsyncWebServer server(80);

const size_t CAPACITY = 1024;

DynamicJsonDocument globalData(1024);

int team_num = 0;
bool timer = false;
long last_sec = millis();

void redBomb(){
  globalData["bomb1"] = true;
  globalData["bomb2"] = false;
  serializeJson(globalData, Serial);
}

void greeenBomb(){
  globalData["bomb1"] = false;
  globalData["bomb2"] = true;
  serializeJson(globalData, Serial);
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/html", "Resource not found<br><a href=\"/\">back</a>");
}

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
      response->print("<p>This is out captive portal front page.</p>");
      response->printf("<p><a href='http://%s'>Click this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
      response->print("</body></html>");
      request->send(response);
    }
};

void send_success_response(AsyncWebServerRequest * request) {
  AsyncResponseStream *stream = request->beginResponseStream("application/json");
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["result"] = 0;
  serializeJson(jsonDoc, *stream);
  request -> send(stream);
}

void setup() {
  Serial.begin(115200);
  
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP(ssid, password, 2, 0, 3);
  Serial.println("Wifi started");
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  globalData["num1"] = 0;
  globalData["num2"] = 0;
  globalData["time"] = false;
  globalData["bomb1"] = false;
  globalData["bomb2"] = false;
  globalData["reset"] = false;
  globalData["stop"] = true;
  globalData["bomb_time"] = 2;
  
  
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    globalData["reset"] = true;
    globalData["num1"] = 0;
    globalData["num2"] = 0;
    globalData["time"] = false;
    globalData["bomb1"] = false;
    globalData["bomb2"] = false;
    globalData["stop"] = true;
    
    serializeJson(globalData, Serial);
    send_success_response(request);
  });
  
  server.on("/member/count/increment/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    int i = globalData["num1"];
    globalData["num1"] = i + 1;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });
  
  server.on("/member/count/increment/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    int i = globalData["num2"];
    globalData["num2"] = i + 1;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/member/count/decrement/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    int i = globalData["num1"];
    globalData["num1"] = i - 1;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });
  
  server.on("/member/count/decrement/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    int i = globalData["num2"];
    globalData["num2"] = i - 1;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/member/count/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    int params = request->params();
    AsyncWebParameter* p = request->getParam(0);
    globalData["num1"] = String(p->value().c_str()).toInt();
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/member/count/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    int params = request->params();
    AsyncWebParameter* p = request->getParam(0);
    globalData["num2"] = String(p->value().c_str()).toInt();
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/bomb/1", HTTP_GET, [](AsyncWebServerRequest * request) {
    globalData["bomb1"] = true;
    globalData["bomb2"] = false;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/bomb/2", HTTP_GET, [](AsyncWebServerRequest * request) {
    globalData["bomb1"] = false;
    globalData["bomb2"] = true;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });
  
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest * request) {
    int params = request->params();
    AsyncWebParameter* p = request->getParam(0);
    globalData["time_num"] = String(p->value().c_str()).toInt();
    globalData["time"] = true;
    globalData["stop"] = false;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/bomb/time", HTTP_GET, [](AsyncWebServerRequest * request) {
    int params = request->params();
    AsyncWebParameter* p = request->getParam(0);
    globalData["bomb_time"] = String(p->value().c_str()).toInt();
    serializeJson(globalData, Serial);
    send_success_response(request);
  });
  
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest * request) {
    globalData["stop"] = true;
    serializeJson(globalData, Serial);
    send_success_response(request);
  });

  server.on("/get/info", HTTP_GET, [](AsyncWebServerRequest * request) {
    serializeJson(globalData, Serial);
    AsyncResponseStream *stream = request->beginResponseStream("application/json");
    serializeJson(globalData, *stream);
    request -> send(stream);
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
  Serial.println("Server started");
}

void loop() {
  dnsServer.processNextRequest();  
  if (millis() - last_sec > 1000) {
    if (globalData["reset"]){
      globalData["reset"] = false; 
    }
    last_sec = millis();
  }
}
