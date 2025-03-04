#pragma once

#ifndef MAINLIB_H
#define MAINLIB_H

#include <Arduino.h>

class Item {
    public:
      Item* __next = NULL;
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
      virtual void serial_print();
    protected:
      boolean active = false;
};

/**
 * Liefert die vergangenen Millisekunden seit time zurueck.
 */
extern int since(int time);

class Pin : public Item {
    public:
      int pin;
      int value = -1;
      String _name = "";
      String mode;
      String comment = "";
      /**  dir="in"|"out" */
		  Pin(int pin, String dir, String* _name = NULL);
      virtual bool cmd(String args[]);
      virtual String name();
      virtual String type();
      virtual String val(String name);
      virtual void loopActive();
      virtual void page(Print* out, String param);
      virtual void pageDetail(Print* out);
      virtual void serial_print();
    protected:
      void setupPinMode(int mode); 
      void writeDigital(int val);
};

class HotPin : public Item {
    public:
      int pin;
      int value = -1;
      String _name;
      int lasttrigger = 0;
      int threshold = 500;
		  HotPin(int pin);
      virtual bool cmd(String args[]);
      virtual String name();
      virtual String type();
      virtual String val(String name);
      virtual void loopActive();
      virtual void page(Print* out, String param);
      virtual void pageDetail(Print* out);
};

class AnalogPin : public Item {
    public:
      int pin;
      int value = -1;
      bool print_serial = false;
      String _name = "";
      String comment = "";
		  AnalogPin(int pin);
      virtual bool cmd(String args[]);
      virtual String name();
      virtual String type();
      virtual String val(String name);
      virtual void loopActive();
      virtual void page(Print* out, String param);
      virtual void pageDetail(Print* out);
      virtual void serial_print();
};


extern int getMainDelay();
extern void processCommand(String cmd);
extern void processScript(String cmd, bool loop=false);
extern void nx_println(String str);
extern void nx_init(String name);
extern String nx_name();
extern void items_loop(bool serial_print=false);
extern boolean items_cmd(String args[]);
extern Item* add_item(Item* item);
extern Item* item_get_root();
extern Item* item_get(String name);
extern void nx_script();

#endif
