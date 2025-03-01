
#include <Arduino.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include "AsyncUDP.h"
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
#ifdef NX_NATIVE
  #include <HttpClient.h>
  #include <WiFi.h>
#endif

#include <nxmc.h>
#include <nxwifi.h>


boolean item_loaded = false;
DNSServer dnsServer;
IPAddress local_IP(10,42,0,1);
IPAddress gateway(10,42,0,1);
IPAddress subnet(255,255,255,0);
String wlan_ssid;
String wlan_password;

String wget(String url) {
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
        return payload;  
    #endif
    #ifdef ESP8266
        WiFiClient client;
        HTTPClient http;
        http.begin(client, url.c_str());
        int httpResponseCode = http.GET();  
        http.end(); 
        return ""; 
    #endif 
    //    #endif
}


void wifi_commands(bool serial_output) {
  if (!item_loaded) {
      NxWifi* item = new NxWifi();
      item->serial_output = serial_output;
      item->activate();
      add_item(item);
      item_loaded = true;
    }
}

void wifi_configure(String ssid, String pw) {
  wlan_ssid = ssid;
  wlan_password = pw;
}

boolean wlanConnect(String ssid, String password, boolean serial_output) {
    if (serial_output) {
        processCommand("rs485_start");
        Serial.print("Connecting To ");
        Serial.println(ssid);
        processCommand("rs485_end");
    }
    wifi_commands(serial_output);
    wlan_ssid = ssid;
    wlan_password = password;
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


void wifi_ap(String ssid, String pw, boolean serial_output) {
    wifi_commands(serial_output);
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


class NxUDP : public Item { 
 public:
    String nparam = "";
    String ncmd = "udp";
    #ifdef ESP32
    AsyncUDP udp;
    #endif
    int i = 0;
    int m = 128;
    void init() {}
    virtual void setup() override {}
    void sendInfo() {
      this->send((String("nxudp ") + nx_name() + String(" ") + WiFi.localIP().toString()));
    }
    void send(String s) {
        #ifdef ESP32
        this->udp.broadcastTo(s.c_str(), 4242); 
        #else
        WiFiUDP Udp;
        IPAddress ip(255, 255, 255, 255);
        Udp.beginPacket(ip, 4242);
        Udp.print(s);
        Udp.endPacket();
        #endif
    }
    void loopActive() override {
       if (this->ncmd.equals("udp")) {
          if (i % this->m == 0) {
            this->sendInfo();
          }
          i++;
       }
    }
    String name() override {
        return "udp";
    }
    virtual void page(Print* out, String param) override {
        out->print("UDP Port: 4242");
       
    }
    virtual bool cmd(String args[]) override {
      if (args[0].equals("udp")) { //
        this->ncmd = "udp";
        return true;
      } else if (args[0].equals("udp_stop")) {
        this->ncmd = "stop";
        return true;
      } else if (args[0].equals("udp_send")) { //
         this->send(args[1].c_str());
        return true;
      } else if (args[0].equals("udp_m")) { //
        this->m = args[1].toInt();
        return true;
      } 
      if (args[0].equals("response")) {
        this->send(String("response ") + String("nxmc") + " " + args[1]);
      }
      return Item::cmd(args); 
    }
};


class WifiConnect : public Item { 
 public:
    String ssid = "";
    String pw = "";
    bool ifconnected = false;
    WifiConnect(String ssid, String pw, bool ifconnected) {
        this->ssid = ssid;
        this->pw = pw;
        this->ifconnected = ifconnected;
    }
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
        int wifi_status = WiFi.status();
        if (wifi_status != 3 /*WL_CONNECTED*/) {
            WiFi.mode(WIFI_STA);
            wifi_status = WiFi.begin(this->ssid, this->pw);
        }
    }
    String name() override {
        return ssid;
    }
    virtual void page(Print* out, String param) override {
        out->print("Wifi: ");
        out->print(ssid);
    }
    virtual bool cmd(String args[]) override {
      //if (args[0].equals(ssid)) { //
      //  return true;
      //}
      return Item::cmd(args); 
    }
};


class NxPull : public Item { 
 public:
    String url;
    String mode; 
    int time;
    int last = 0;
    NxPull(String url, String mode, int time) {
        this->url = url;
        this->mode = mode;
        this->time = time;
    }
    void init() {}
    void execute() {
      if (this->mode.equals("wsleep")) {
        processCommand("wifi reconnect");
        delay(10);
      }
      processScript(wget(this->url), true);
      if (this->mode.equals("wsleep")) {
        processCommand("wifi off");
      }
    }
    void loopActive() override {
       if (millis() > this->last + this->time) {
          this->last = millis();
          this->execute();
       }
    }
    String name() override {
        return "nxpull";
    }
    virtual void page(Print* out, String param) override {
        out->print("NxPull");
       
    }
    virtual bool cmd(String args[]) override {
      if (args[0].equals("nxp")) { 
        //this->ncmd = "udp";
        return true;
      } 
      return false;
    }
};


void NxWifi::setup() {}

void NxWifi::loopActive() {
    //dnsServer.processNextRequest();
}
String NxWifi::name() {
    return "NxWifi";
}
void NxWifi::page(Print* out, String param) {
    out->print("NxWifi");
}
String NxWifi::val(String name) {
    if (name.equals("status")) {
        return String(WiFi.status());
    } else if (name.equals("localIP")) {
        return WiFi.localIP().toString();
    } else if (name.equals("rssi")) {
        return String(WiFi.RSSI());
    }
    return "";
}
bool cmd_nxpull(String args[]) {
    add_item(new NxPull(args[1], args[2], args[3].toInt()))->activate();
    return true;
}
bool NxWifi::cmd(String args[]) {
  if (args[0].equals("wifi")) {
    if (args[1].equals("off")) {
        WiFi.mode(WIFI_OFF);
        return true;
    } else if (args[1].equals("udp")) {
      add_item(new NxUDP())->activate();
    } else if (args[1].equals("connect")) {
      wlanConnect(args[2].c_str(), args[3].c_str(), false);
    } else if (args[1].equals("reconnect")) {
      wlanConnect(wlan_ssid, wlan_password, false);
    } else if (args[1].equals("ap")) {
      wifi_ap(args[2], args[3], false);
      return true;
    }
  } else if (args[0].equals("nxpull")) {
    return cmd_nxpull(args);
  } else if (args[0].equals("wificonnect")) {
    add_item(new WifiConnect(args[1], args[2], args[3].toInt()==1))->activate();
    return true;
  }
  return false;
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

void wifi_mdns(String name) {
  MDNS.begin(name);
}

void add_udp() {
  Item* itm = new NxUDP();
  itm->activate();
  add_item(itm);
}

void wifi_npy() {
  IPAddress ip = WiFi.localIP();
  if (ip[0] == 10 && ip[1] == 42 && ip[2] == 0) {
    wget("http://10.42.0.1:7070/?type=network&scan_ip=" + WiFi.localIP());
    //wget("http://10.42.0.1:7070/?type=esp_up&ip=" + WiFi.localIP());
  }
}

void wifi_list() {
  int n = WiFi.scanNetworks(false, true);
  if(n >= 0) {
    Serial.println("Wifis: "+n);
    for (int i = 0; i < n; i++) {
      //processResponse("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
      Serial.print(WiFi.SSID(i).c_str());
      Serial.print("   Channel: ");
      Serial.print(WiFi.channel(i));
      Serial.print("   RSSI: ");
      Serial.print(WiFi.RSSI(i));
      Serial.print("   Enc: ");
      Serial.println(WiFi.encryptionType(i));
    }
    WiFi.scanDelete();
  }
}

bool wifi_is_present(String name) {
  int n = WiFi.scanNetworks(false, true);
  if(n >= 0) {
    for (int i = 0; i < n; i++) {
      if (WiFi.SSID(i).equals(name)) {
        return true;
      }
    }
  }
  return false;
}