#pragma once

#ifndef MAINLIB_H
#define MAINLIB_H

#include <Arduino.h>

extern int getMainDelay();

class Item {
    public:
      Item* __next;
		  //Item();
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
    protected:
      boolean active = false;
};

#endif
