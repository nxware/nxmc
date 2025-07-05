/**
 *  #include <nxwebserver.h>
 */

#ifndef NX_NOWIFI

#ifndef M_PAGES_ASYNC_H
#define M_PAGES_ASYNC_H

#include <Arduino.h>

#ifdef ESP32
  #include <ESPAsyncWebServer.h>
#elif defined(ESP8266)
  #include <ESPAsyncWebServer.h>
#else
  #include <AsyncWebServer_RP2040W.h>
#endif

extern AsyncWebServer* webserver_start();
extern void ws_feature_add(String feature_name);

extern String setup_button(String cmd, String title);

extern String request_param(String name);

extern void pages_response_async(const char*);

#endif

#endif