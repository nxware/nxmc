/**
 *  https://github.com/me-no-dev/ESPAsyncWebServer
 */ 

#ifndef NX_NOWIFI

#include <Arduino.h>
#ifdef ESP32
  #include <ESPAsyncWebServer.h>
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <esp_wifi.h>
  #include <Preferences.h> // ESP8266 https://github.com/vshymanskyy/Preferences
#elif defined(ESP8266)
  #include <ESPAsyncWebServer.h>
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <esp_wifi.h>
#else
  #include <AsyncWebServer_RP2040W.h>
#endif

#include "time.h"

#include "nxmc.h"

#ifdef ESP32
Preferences w_preferences;
#endif

AsyncWebServer serverAsync(80);
AsyncWebServerRequest *requestAsync = NULL;
int last_request = 0;

const char* PARAM_MESSAGE = "message";

String feature_str = "\"nxesp\",\"nxmc\"";


String request_param(String name) {
    if (requestAsync != NULL) {
        const AsyncWebParameter* param = requestAsync->getParam(name);
        if (param != NULL) {
            return param->value();
        }
    } 
    return "";
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void pages_response_async(const char* str) {
    if (requestAsync != NULL) {
        requestAsync->send(200, "text/plain", str);
    }
}

void page_scan_async(AsyncWebServerRequest *request) {  // https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/scan-examples.html
    // funktioniert da man das wlan trennen muss
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    response->println("Wifi Scan");
    #ifdef ESP8266
    int n = WiFi.scanNetworks(false, true);
    if(n >= 0) {
        response->printf("%d network(s) found\n", n);
        for (int i = 0; i < n; i++) {
            response->printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
        }
        WiFi.scanDelete();
    }
    #endif
    //int n = WiFi.scanNetworks(false, true);
    //if(n >= 0) {
    //    Wi
    //}
    request->send(response);
}

void page_ips_async(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("text/plain");
  unsigned char number_client;
  struct station_info *stat_info;
  struct ip4_addr *IPaddress;
  IPAddress address;
  int i=1;
  #ifdef ESP8266
  number_client= wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  stat_info = wifi_softap_get_station_info();
  response->print("Client-Count= ");
  response->println(number_client);
  while (stat_info != NULL) {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;
    response->print("ClientId=  ");
    response->print(i);
    response->print("; IP= ");
    response->print((address));
    response->print("; MAC= ");
    response->print(stat_info->bssid[0],HEX);
    response->print(":");
    response->print(stat_info->bssid[1],HEX);
    response->print(":");
    response->print(stat_info->bssid[2],HEX);
    response->print(":");
    response->print(stat_info->bssid[3],HEX);
    response->print(":");
    response->print(stat_info->bssid[4],HEX);
    response->print(":");
    response->print(stat_info->bssid[5],HEX);
    stat_info = STAILQ_NEXT(stat_info, next);
    i++;
    response->println();
  }
  #endif
  request->send(response);
}

void page_items(AsyncWebServerRequest *request) {
    last_request = millis();
    Item* item = item_get_root();
    requestAsync = request;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    while (item != NULL) {
        response->print("<div class=\"nxitem\"><div style='font-size: 120%;' class='item_name'>");
        response->print(item->name());
        if (item->isActive()) { response->print(" (active)"); }
        // TODO /item?name=pin26
        response->print("</div>\n");
        String param = "";
        if (request->hasParam(item->name())) {
            param = request->getParam(item->name())->value();
        }
        item->page(response, param);
        response->print("</div>");
        item = item->__next;
    }
    request->send(response);
}

void page_item(AsyncWebServerRequest *request) {
    last_request = millis();
    Item* item = item_get(request->getParam("name")->value());
    requestAsync = request;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    if (item != NULL) {
        response->println("Item");
        item->pageDetail(response);
    } else  {
        response->println("Error: Item not found.");
    }
    request->send(response);
}

void page_val(AsyncWebServerRequest *request) {
    last_request = millis();
    Item* item = item_get(request->getParam("name")->value());
    requestAsync = request;
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    if (item != NULL) {
        String vname = request->getParam("value")->value();
        response->print(item->val(vname));

        
    }
    request->send(response);
}




void page_status_async(AsyncWebServerRequest *request) {
    last_request = millis();
 /*request->send(200, "text/plain", 
    (String("BootID: ") + String(bootid) + String("\n") +
#ifdef NX_DISPLAY
    "Compile Option NX_DISPLAY\n" + 
#endif
#ifdef NX_SERVER
    "Compile Option NX_SERVER\n" + 
#endif
#ifdef NX_WIFI
    "Compile Option NX_WIFI\n" + 
#endif
#ifdef NX_AP
    "Compile Option NX_AP\n" + 
#endif
#ifdef NX_CO2
    "Compile Option NX_CO2\n" + 
#endif
#ifdef NX_SERVER
     "MAC: "+WiFi.macAddress()+"\n"
#endif
    "Display: " + display_status + "\n").c_str()
  );    */
   AsyncResponseStream *response = request->beginResponseStream("text/plain");
   //response->print("BootID: ");
   //response->println(bootid);
   #ifdef NX_DISPLAY
   //response->println("Compile Option NX_DISPLAY");
   #endif
   #ifdef ESP8266
   //response->println("Chip: ESP8266");
   //response->print("ChipId: ");
   //response->println(ESP.getChipId());
   //response->print("BootMode: ");
   //response->println(ESP.getBootMode());
   //response->print("FlashChipSize: ");
   //response->println(ESP.getFlashChipSize());
   //response->print("FlashChipId: ");
   //response->println(ESP.getFlashChipId());
   //response->print("Vcc: ");
   //response->println(ESP.getVcc());
   //response->print("ResetInfo: ");
   //response->println(ESP.getResetInfo());
   #endif
   #ifdef ESP32
   //response->println("Chip: ESP32");
   //response->print("ChipId: ");
   //response->println((uint16_t)ESP.getEfuseMac()>>32);
   #endif
   //response->print("FreeHeap: ");
   //response->println(ESP.getFreeHeap());
   //response->print("Serial Baud: ");
   //response->println(NX_BAUD);
   //response->print("NX-Delay: ");
   //response->println(getMainDelay());
   #ifdef NX_WIFI
   //response->print("Wifi Connected: ");
   //if (WiFi.isConnected())  response->println("Yes"); else response->println("No");
   //response->print("Wifi Mode: ");
   //response->println(WiFi.getMode()); // WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3, /* these two pseudo modes are experimental: */ WIFI_SHUTDOWN = 4, WIFI_RESUME = 8
   //response->print("Wifi MAC: ");
   //response->println(WiFi.macAddress());
   //response->print("Wifi RSSI: ");
   //response->println(WiFi.RSSI());
   #endif

   /*for (int i = 0; i < 16; i++) {
    uint8_t pin = i;
    uint8_t port = digitalPinToPort(pin);
    volatile uint32_t* out = portOutputRegister(port);
    uint8_t bm = digitalPinToBitMask(pin);
    response->println(pin);
    //response->print("portOutputRegister: ");
    //response->println((int)out, HEX);
    response->print("portOutputRegisterBin: ");
    response->println((int)out, 2);
    response->print("portOutputRegisterMaskBin: ");
    response->println((int)GPC12    , 2);
    }*/
    #ifdef ESP8266
    /*response->print("GPC12: ");
    response->println((int)GPC12    , 2);
    response->print("GPF12  : ");
    response->println((int)GPF12      , 2);
    response->print("GPC13: ");
    response->println((int)GPC13    , 2);
    response->print("GPF13  : ");
    response->println((int)GPF13      , 2);
                                                //               GPIO12
    response->print("GPO  (GPIO_OUT R/W (Output Level)): "); // 11000000000001
    response->println((int)GPO      , 2);                    //              GPIO0
    response->print("GPOS  (GPIO_OUT_SET WO): ");
    response->println((int)GPOS      , 2); */
    #endif

    //#define GPOC   ESP8266_REG(0x308) //GPIO_OUT_CLR WO
    //#define GPE    ESP8266_REG(0x30C) //GPIO_ENABLE R/W (Enable)
    //#define GPES   ESP8266_REG(0x310) //GPIO_ENABLE_SET WO
    //#define GPEC   ESP8266_REG(0x314) //GPIO_ENABLE_CLR WO
    //#define GPI    ESP8266_REG(0x318) //GPIO_IN RO (Read Input Level)
    //#define GPIE   ESP8266_REG(0x31C) //GPIO_STATUS R/W (Interrupt Enable)
    //#define GPIES  ESP8266_REG(0x320) //GPIO_STATUS_SET WO
    //#define GPIEC  ESP8266_REG(0x324) //GPIO_STATUS_CLR WO

   response->print("ok"); 
   request->send(response);
}

void page_cmd(AsyncWebServerRequest *request) {
    last_request = millis();
    const AsyncWebParameter* redirect = request->getParam("redirect");
    if (redirect == nullptr) {
        requestAsync = request;
    }
    processCommand(request->getParam("cmd")->value());
    // TODO redirect
    requestAsync = NULL;
    if (redirect != nullptr) {
        request->redirect("http://" + WiFi.localIP().toString());
    }
}

void page_time_async(AsyncWebServerRequest *request) {
    last_request = millis();
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    #ifdef ESP32
    response->println("Time: ");
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      response->println("Faild to get time");
    } else {
      response->println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
    }
    #endif
    request->send(response);
}

String setup_button(String cmd, String title) {
    return "<form action='cmd'><input type='hidden' name='redirect' value='1' /><input name='cmd' type='hidden' text='"+cmd+"' /><input type='submit' value='"+title+"' /></form>";
}

void page_index_async(AsyncWebServerRequest *request) {
    last_request = millis();
    //request->send(200, "text/plain", NX_VERSION);
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->println("<html>");
    response->println("<head>");
    response->println("<style>");
    response->println(" body { font-family: sans-serif; background-color: #aaa;}");
    response->println(" #main { width: 900px; margin: auto; padding: 5px; border: 1px #000 solid; background-color: #eee;}");
    response->println("</style>");
    response->println("</head>");
    response->println("<body>");
    response->println("<div id='main'>");
    response->print("<h1>NxESP: ");
    response->print(nx_name());
    response->println("</h1>");
    response->print("<h2>");
    //response->println(NX_VERSION);
    response->println("</h2>");
    #ifdef NX_CO2
    response->print(co2_get_percent());
    response->print("%");
    #endif
    response->println("<ul>");
    response->println("<li><a href='status'>Status</a></li>");
    response->println("<li><a href='uptime'>Uptime</a></li>");
    response->println("<li><a href='rssi'>RSSI</a></li>");
    response->println("<li><a href='items'>Items</a></li>");
    response->println("<li><a href='display'>Display</a></li>");
    response->println("</ul>");
    //part_pins(request, response);
    response->println("<form action='cmd'><input type='hidden' name='redirect' value='1' /><input name='cmd' type='text' /><input type='submit' value='exec' />");
    response->println("</form>");

    #ifdef WEMOS_D1
    response->println("Wemos D1");
    response->println(setup_button("gpio init_out 14", "Init S1/14 Out"));
    response->println(setup_button("gpio init_out 2", "Init S2/2 Out"));
    response->println(setup_button("gpio init_out 12", "Init S3/12 Out"));
    response->println(setup_button("gpio init_out 13", "Init S4/13 Out"));
    response->println(setup_button("gpio init_out 0", "Init S5/0 Out"));
    response->println(setup_button("gpio init_out 16", "Init S6/16 Out"));
    response->println(setup_button("gpio init_out 19", "Init S7/19 Out"));
    response->println(setup_button("gpio init_out 15", "Init S8/15 Out"));
    #endif
    #ifdef ESP32
    response->println("ESP32");
    response->println(setup_button("gpio init_out 2", "Init Int LED Out"));
    response->println(setup_button("gpio init_out 26", "Init S1/26 Out"));
    response->println(setup_button("gpio init_out 18", "Init S2/18 Out"));
    response->println(setup_button("gpio init_out 19", "Init S3/19 Out"));
    response->println(setup_button("gpio init_out 23", "Init S4/23 Out"));
    response->println(setup_button("gpio init_out 5", "Init S5/5 Out"));
    response->println(setup_button("gpio init_out 27", "Init S6/27 Out"));
    response->println(setup_button("gpio init_out 25", "Init S7/25 Out"));
    response->println(setup_button("gpio init_out 32", "Init S8/32 Out"));
    #endif
    
    response->println("</div>");
    response->println("</body>");
    response->println("</html>");
    request->send(response);
}

void ws_feature_add(String feature_name) {
    feature_str += ", \"" + feature_name + "\"";
}

String features() {
    #ifdef ESP32

    #endif
    return "[" + feature_str + "]";
}

void page_info(AsyncWebServerRequest *request) {
    last_request = millis();
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->println("{");
    response->print(" \"name\": \""); 
    response->print(nx_name());
    response->println("\",");
    uint8_t baseMac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK) {
        response->print(" \"mac\": \""); 
        response->printf("%02x:%02x:%02x:%02x:%02x:%02x",  baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
        response->println("\",");
    } 
    response->print(" \"features\": ");
    response->print(features());
    response->println(",");
    #if ESP32
      response->println(" \"chip\": \"esp32\",");
    #endif
    response->print("\"success\": true");
    response->println("}");
    request->send(response);
}

void page_names(AsyncWebServerRequest *request) {
    last_request = millis();
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->println("[");
    Item* root = item_get_root();
    if (root != NULL) {
        Item* current = root;
        while (true) {
            response->print("\"");
            response->print(current->name());
            response->print("\"");
            current = current->__next;
            if (current == NULL) {
                break;
            } else {
                response->println(", ");
            }
        }  
    }
    response->println("]");
    request->send(response);
}

void page_name_post(AsyncWebServerRequest *request) {
    #ifdef ESP32
     w_preferences.begin("nx", false);
     int params = request->params();
     for(int i=0;i<params;i++){
       const AsyncWebParameter* p = request->getParam(i);
       w_preferences.putString("name", p->value());
     }
     //const AsyncWebParameter* j = request->getParam((size_t)0); // 1st parameter
     //w_preferences.putString("script", j->value());
     //w_preferences.putString("script", "set " + String(params));
     w_preferences.end();
   #endif
   AsyncResponseStream *response = request->beginResponseStream("application/json");
   response->println("{}");
   request->send(response);
 }

void page_script_post(AsyncWebServerRequest *request) {
   #ifdef ESP32
    w_preferences.begin("nx", false);
    int params = request->params();
    for(int i=0;i<params;i++){
      const AsyncWebParameter* p = request->getParam(i);
      w_preferences.putString("script", p->value());
    }
    //const AsyncWebParameter* j = request->getParam((size_t)0); // 1st parameter
    //w_preferences.putString("script", j->value());
    //w_preferences.putString("script", "set " + String(params));
    w_preferences.end();
  #endif
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  response->println("{}");
  request->send(response);
}

void page_script(AsyncWebServerRequest *request) {
   #ifdef ESP32
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    w_preferences.begin("nx", false);
    response->println(w_preferences.getString("script", ""));
    w_preferences.end();
    request->send(response);
  #else
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->println("{}");
    request->send(response);
  #endif
}

AsyncWebServer* webserver_start() {
    serverAsync.on("/", HTTP_GET, [](AsyncWebServerRequest *request){page_index_async(request);});
    //serverAsync.on("/bootid", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", String(bootid));});
    serverAsync.on("/mac", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", WiFi.macAddress());});
    serverAsync.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", String(WiFi.RSSI()));});
    serverAsync.on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", String(millis()));});
    serverAsync.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request){page_cmd(request);});
    serverAsync.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){ page_status_async(request); });
    serverAsync.on("/ips", HTTP_GET, [](AsyncWebServerRequest *request){ page_ips_async(request); });
    serverAsync.on("/generate_204", [](AsyncWebServerRequest *request){ page_index_async(request); }); 
    serverAsync.on("/scan", [](AsyncWebServerRequest *request){ page_scan_async(request); });
    serverAsync.on("/items", HTTP_GET, [](AsyncWebServerRequest *request){ page_items(request);  });
    serverAsync.on("/item", HTTP_GET, [](AsyncWebServerRequest *request){ page_item(request);  });
    serverAsync.on("/val", HTTP_GET, [](AsyncWebServerRequest *request){ page_val(request);  });
    serverAsync.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){ page_time_async(request); });
    serverAsync.on("/features", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "application/json", features()); });
    serverAsync.on("/name", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "application/json", nx_name()); });
    serverAsync.on("/name", HTTP_POST, [](AsyncWebServerRequest *request){ page_name_post(request); });
    serverAsync.on("/names", HTTP_GET, [](AsyncWebServerRequest *request){ page_names(request); });
    serverAsync.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){ page_info(request); });
    serverAsync.on("/script", HTTP_POST, [](AsyncWebServerRequest *request){ page_script_post(request); });
    serverAsync.on("/script", HTTP_GET, [](AsyncWebServerRequest *request){ page_script(request); });
    serverAsync.onNotFound(notFound);
    serverAsync.begin();
    return &serverAsync;
}

#endif