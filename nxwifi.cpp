
#include <Arduino.h>

#ifdef ESP32
  #include <HTTPClient.h>
  #include "esp_wifi.h"
    #include <ESPmDNS.h>
  #include <DNSServer.h>
#endif

#ifdef ESP8266
  #include <ESP8266HTTPClient.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#endif

DNSServer dnsServer;
IPAddress local_IP(10,42,0,1);
IPAddress gateway(10,42,0,1);
IPAddress subnet(255,255,255,0);

void wget(String url) {
    //  #ifdef NX_WIFI
    #ifdef ESP32
        HTTPClient http;
        http.begin(url);
        int httpResponseCode = http.GET();
        String payload = "HTTP Error";
        if (httpResponseCode > 0) { //Check the returning code
          payload = http.getString();   // Get the request response payload
        }
        http.end();  
    #endif
    #ifdef ESP8266
        WiFiClient client;
        HTTPClient http;
        http.begin(client, url.c_str());
        int httpResponseCode = http.GET();  
        http.end();  
    #endif 
    //    #endif
}

boolean wlanConnect(const char* ssid, const char* password, boolean serial_output) {
    if (serial_output) {
          Serial.print("Connecting To ");Serial.println(ssid);
    }
    //WL_IDLE_STATUS      = 0,
    //WL_NO_SSID_AVAIL    = 1,
    //WL_SCAN_COMPLETED   = 2,
    //WL_CONNECTED        = 3,
    //WL_CONNECT_FAILED   = 4,
    //WL_CONNECTION_LOST  = 5,
    //WL_DISCONNECTED     = 6
    WiFi.mode(WIFI_STA);
    //for(int i = 0; i < 3;i++) {
      int wifi_status = WiFi.begin(ssid, password); // #######################################################################
      if (wifi_status == WL_CONNECTED) {
        if (serial_output) {
          Serial.print("Connected To ");Serial.println(ssid);
        }
        return true;
      }
      delay(3000);
      if (serial_output) {
        Serial.print("Wifi Status: ");Serial.println(wifi_status);
      }
      for(int j = 0; j < 3;j++) {
        int wifi_status_b = WiFi.status();
        if (serial_output) {
            Serial.print("Wifi Status: ");Serial.println(wifi_status_b);
        }
        if (wifi_status_b == WL_CONNECTED) {
          return true;
        }
        delay(2000);
      }
      delay(300);
    //}
  return false;
}

void wifi_ap(String ssid, String pw) {
    // mehr zu special events: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html#generic-class
    WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP ipInfo) { // As soon WiFi is connected, start NTP Client
        Serial.printf("Got IP: %s\r\n", WiFi.localIP().toString().c_str());
        //NTP.begin("pool.ntp.org", 1, true);
        //NTP.setInterval(63);
    });   
    WiFi.onSoftAPModeStationConnected(&onStationConnected);

    #ifdef ESP8266
    WiFi.mode(WIFI_AP);
    #endif
    //uint8_t mac[WL_MAC_ADDR_LENGTH];
    //WiFi.softAPmacAddress(mac);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid, pw);
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(/*DNS_PORT*/53, "*", local_IP); // fuer ein Captive portal"
}