/**
 *  #include <nxwebserver.h>
 */

#ifndef M_PAGES_ASYNC_H
#define M_PAGES_ASYNC_H

#include <Arduino.h>

#ifdef RASPBERRY_PI_PICO_W
  #include <AsyncWebServer_RP2040W.h>
#else
  #include <ESPAsyncWebServer.h>
#endif

extern AsyncWebServer* webserver_start();

extern String setup_button(String cmd, String title);

extern String request_param(String name);

extern void pages_response_async(const char*);

#endif