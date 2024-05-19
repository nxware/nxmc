/**
 *  #include <nxwebserver.h>
 */

#ifndef M_PAGES_ASYNC_H
#define M_PAGES_ASYNC_H

#include <Arduino.h>

extern void webserver_start();

extern Strimg request_param(String name);

extern void pages_response_async(const char*);

#endif