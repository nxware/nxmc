/**
 *  #include <nxwebserver.h>
 */

#ifndef M_PAGES_ASYNC_H
#define M_PAGES_ASYNC_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer* webserver_start();

extern String setup_button(String cmd, String title);

extern String request_param(String name);

extern void pages_response_async(const char*);

#endif