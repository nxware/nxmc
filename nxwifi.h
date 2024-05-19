
#ifndef NXWIFI_H
#define NXWIFI_H

#include <Arduino.h>

extern void wget(String url);

extern boolean wlanConnect(const char* ssid, const char* password, boolean serial_output);
extern void wifi_ap(String ssid, String pw);

#endif