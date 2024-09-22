
#ifndef NXWIFI_H
#define NXWIFI_H

#include <Arduino.h>

class NxWifi : public Item { 
  public:
    virtual void setup() override;
    void loopActive() override;
    String name() override;
    virtual void page(Print* out, String param) override;
    virtual String val(String name) override;
    virtual bool cmd(String args[]) override;
};

extern String wget(String url);
extern void wifi_commands();

extern boolean wlanConnect(const char* ssid, const char* password, boolean serial_output);
extern void wifi_ap(String ssid, String pw);
extern void wifi_ap_sta(String ap_ssid, String ap_pw, String other_ssid, String other_pw);

extern void wifi_list();
extern bool wifi_is_present(String name);

extern void wifi_mdns(String name);
extern void wifi_npy();
extern void add_udp();

#endif