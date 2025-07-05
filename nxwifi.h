
#ifndef NX_NOWIFI

#ifndef NXWIFI_H
#define NXWIFI_H

#include <Arduino.h>

class NxWifi : public Item { 
  public:
    bool serial_output = false;
    virtual void setup() override;
    void loopActive() override;
    String name() override;
    virtual void page(Print* out, String param) override;
    virtual String val(String name) override;
    virtual bool cmd(String args[]) override;
};

extern String wget(String url);
extern void wifi_commands(bool serial_output);

extern boolean wlanConnect(String ssid, String password, boolean serial_output=true);
extern void wifi_configure(String ssid, String pw);
extern void wifi_ap(String ssid, String pw, boolean serial_output=true);
extern void wifi_ap_sta(String ap_ssid, String ap_pw, String other_ssid, String other_pw);

extern void wifi_list();
extern bool wifi_is_present(String name);
extern bool wifi_is_connected();

extern void wifi_mdns(String name);
extern void wifi_npy();
extern void add_udp();

#endif

#endif