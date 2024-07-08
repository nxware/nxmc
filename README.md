# nxmc

bibliothek für Embedded-Systeme (ESP32).

## nxmc.h

Hauptsystem

 - `class Item`
 - `int getMainDelay();`
 - `void processCommand(String cmd);`
 - `void items_loop();` - Wird in `loop()` aufgerufen
 - `boolean items_cmd(String args[]);`
 - `Item* add_item(Item* item);`
 - `Item* item_get_root();`
 - `Item* item_get(String name);`

Item ermöglicht es  mehrere Loops auf einem MicroController zu betreiben.
```
class Item:
      virtual void setup();
      virtual void loop();
      virtual void loopActive();
      virtual bool cmd(String args[]);
      virtual String name();
      virtual String type();
      virtual String val(String name);
      virtual void page(Print* out, String param);
      virtual void pageDetail(Print* out);
      boolean isActive();
      virtual void activate();
```

## nxwifi.h

##nxcmd

 - `wifi udp`

## nxwebserver.h
```
#include <nxwebserver.h>
```

 - `void webserver_start();` - In `setup()` aufrufen
 - `String request_param(String name);`
 - `void pages_response_async(const char*);`
 - `Item::page(...)`

Routen:
- `/`
- `/uptime`
- `/items`
- `/val?name=ItemName&value=ValueName`


## nxdisplay.h
```
void display_start();
void display_clear();
void display_display();

void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void display_value(float value, int size, int x, int y);
void display_str(String value, int size, int x, int y);
```

## Deployment

Beispiel der einbindung in einem platform.io Projekt:
```
[env:d1_mini]
platform = espressif8266
board = d1_mini
lib_deps = 
  ESP8266HTTPClient
  ESP8266WiFi
  ESP8266HTTPClient
  me-no-dev/ESPAsyncTCP
  me-no-dev/ESP Async WebServer
  https://github.com/nxware/nxmc
```

```
pio pkg update
```