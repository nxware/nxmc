#pragma once

#ifndef MAINLIB_H
#define MAINLIB_H

#include <Arduino.h>

class Item {
    public:
      Item* __next;
		  //Item();
      virtual void setup();
      virtual void loop();
      virtual void loopActive();
      virtual void display();
      virtual bool cmd(String args[]);
      virtual String name();
      virtual String type();
      virtual String val(String name);
      virtual void page(Print* out, String param);
      virtual void pageDetail(Print* out);
      boolean isActive();
      virtual void activate();
    protected:
      boolean active = false;
};


extern int getMainDelay();
extern void processCommand(String cmd);
extern void items_loop();
extern boolean items_cmd(String args[]);
extern Item* add_item(Item* item);
extern Item* item_get_root();
extern Item* item_get(String name);

#endif
