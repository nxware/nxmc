
#include <Arduino.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <HTTPClient.h>
  #include "esp_wifi.h"
  #include <ESPmDNS.h>
  #include <DNSServer.h>
#endif

#ifdef ESP8266
  #include <ESP8266HTTPClient.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <DNSServer.h>
#endif

#include <nxmc.h>


boolean item_loaded = false;
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

class NxWifi : public Item { 
  public:
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
        dnsServer.processNextRequest();
    }
    String name() override {
        return "NxWifi";
    }
    virtual void page(Print* out, String param) override {
        out->print("NxWifi");
    }
    virtual String val(String name) override {
        if (name.equals("status")) {
            return String(WiFi.status());
        } else if (name.equals("localIP")) {
            return WiFi.localIP().toString();
        } else if (name.equals("rssi")) {
            return String(WiFi.RSSI());
        }
        return "";
    }
    virtual bool cmd(String args[]) override {
      if (args[0].equals("wifi")) {
        if (args[1].equals("off")) {
            WiFi.mode(WIFI_OFF);
            return true;
        }
        return true;
      } 
      return false;
    }
};

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
    if (!item_loaded) {
        NxWifi* item = new NxWifi();
        item->activate();
        add_item(item);
    }
    // mehr zu special events: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html#generic-class
    #ifdef ESP32
    //WiFi.onStationModeGotIP([](WiFiEventStationModeGotIP ipInfo) { // As soon WiFi is connected, start NTP Client
    //    Serial.printf("Got IP: %s\r\n", WiFi.localIP().toString().c_str());
        //NTP.begin("pool.ntp.org", 1, true);
        //NTP.setInterval(63);
    //});   
    //WiFi.onSoftAPModeStationConnected(&onStationConnected);
    #endif

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

void wifi_ap_sta(String ap_ssid, String ap_pw, String other_ssid, String other_pw) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(other_ssid, other_pw);
  Serial.print(WiFi.softAP(ap_ssid, ap_pw/*, 1 , 0 , 8*/) ? "AP Ready" : "AP Failed");
  Serial.print("\nSensonetz Access Point IP Adresse ");
  Serial.println(WiFi.softAPIP()); // WiFi.localIP()
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}