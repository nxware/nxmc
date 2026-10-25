
#ifndef NX_NOWIFI

#ifndef NXWIFI_H
#define NXWIFI_H

#include <Arduino.h>
#include <nxmc.h>

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

/**
 * Haelt die Verbindung zu einem WLAN im Hintergrund aufrecht: solange keine
 * Verbindung besteht, wird alle WIFICONNECT_RETRY_MS ein Versuch gestartet,
 * ohne loop() zu blockieren. Ein laufender AP bleibt dabei aktiv (AP+STA),
 * z.B. um nach einem Fallback-AP spaeter doch noch ins WLAN zu kommen.
 * Mehrere Items wechseln sich ab (je Versuch WIFICONNECT_ATTEMPT_MS Zeit).
 * ifconnected=true: den AP abschalten, sobald die Verbindung steht.
 *
 * Befehl: wificonnect ssid pw [1]
 */
class WifiConnect : public Item {
  public:
    String ssid = "";
    String pw = "";
    bool ifconnected = false;
    WifiConnect(String ssid, String pw, bool ifconnected);
    void loopActive() override;
    String name() override;
    virtual void page(Print* out, String param) override;
  private:
    unsigned long last_attempt = 0;
    bool attempted = false;
};

extern String wget(String url);
extern void wifi_commands(bool serial_output);

extern boolean wlanConnect(String ssid, String password, boolean serial_output=true);
extern boolean wlanConnect2(String ssid, String password, String ssid2, String password2, boolean ap, boolean serial_output=true);
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