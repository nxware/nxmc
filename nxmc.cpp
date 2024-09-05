
#include <Arduino.h>

#include <time.h> // wegen setTime
#include <TimeLib.h> // paulstoffregen/Time

#include <nxmc.h>

/**
 * Liefert die vergangenen Millisekunden seit time zurueck.
 */
int since(int time) {
  return millis()-time;
}

//Item::Item() {}
/** wird beim add_item aufgerufen */void Item::setup() {}
void Item::loop() {
  if (this->active) {
    this->loopActive();
  }
}
void Item::activate() {
  this->active = true;
}
void Item::display() {}
void Item::loopActive() {}
boolean Item::isActive() {
  return this->active;
}
String Item::name() { return "item";}
String Item::type() { return "item";}
bool Item::cmd(String args[]) {
  if (args[0].equals(this->name())) {
    if (args[1].equals("activate")) {
      this->activate();
      return true;
    } else if (args[1].equals("deactivate")) {
      this->active = false;
      return true;
    }
  }
  return false;
}
void Item::page(Print* out, String param) {
  out->println("Item");
}
void Item::pageDetail(Print* out) {}
String Item::val(String name) {
  return "unknown";
}
void Item::serial_print() {}


String exec_button(String cmd, String title) {
    return "<form action='cmd'><input type='hidden' name='redirect' value='1' /><input name='cmd' type='hidden' text='"+cmd+"' /><input type='submit' value='"+title+"' /></form>";
}

Pin::Pin(int pin, String dir, String* _name) {
  this->pin = pin;
  this->_name = String("pin" + String(pin));
  this->mode = dir;
  if (_name != NULL) {
    this->_name = String(_name->c_str());
  }
}
bool Pin::cmd(String args[]) {
  if (this->_name.equals(args[0])) {
    if (args[1].equals("1")) {
      this->value = 1;
    } else if (args[1].equals("0")) {
      this->value = 1;
    } else if (args[1].equals("pulse")) {
      this->mode = "out_pulse";
    } else if (args[1].equals("tone")) {
      this->mode = "out_tone";
    }
    return true;
  } else {
    return false;
  }
}
String Pin::name() {
  return this->_name;
}
String Pin::type() {
  return "Pin";
}
void Pin::loopActive() {
  if (this->mode.equals("init_out")) {
    pinMode(this->pin, OUTPUT);
    this->mode = "out";
  } else if (this->mode.equals("init_in")) {
    pinMode(this->pin, INPUT);
    this->mode = "in";
  }
  if (this->mode.equals("out") && this->value!=-1) {
    digitalWrite(this->pin, this->value);
  } else if (this->mode.equals("in")) {
    this->value = digitalRead(this->pin);
  } else if (this->mode.equals("out_pulse")) {
    digitalWrite(this->pin, 1);
    delay(1000);
    digitalWrite(this->pin, 0);
    this->mode = "out";
  } else if (this->mode.equals("out_tone")) {
    tone(this->pin, 440, 1000);
    digitalWrite(this->pin, 0);
    this->mode = "out";
  } else if (this->mode.equals("out_blink")) {
    this->value = this->value==1 ? 0 : 1;
    digitalWrite(this->pin, this->value);
  }
}
String Pin::val(String name) {
  if (name.equals("mode")) {
    return String(this->mode);
  } else if (name.equals("value")) {
    return String(this->value);
  }
  return "";
}

void Pin::page(Print* out, String param) {
  out->print(this->name());
  out->print("Pin: ");
  out->print(this->pin);
  out->print(" Mode: ");
  out->print(this->mode);
  out->print(" Value: ");
  out->print(this->value);
  out->print(" Comment: ");
  out->print(this->comment);
  out->print("<div>");
  out->print(exec_button(this->name()+" 1", "Ein"));
  out->print(exec_button(this->name()+" 0", "Aus"));
  out->print(exec_button(this->name()+" pulse", "Pulse"));
  out->print("</div>");
}
void Pin::pageDetail(Print* out)  {
  out->print("Pin: ");
  out->print(this->pin);
  out->print("<br />Mode: ");
  out->print(this->mode);
  out->print("<br />Value: ");
  out->print(this->value);
  out->print("<br />Flags: ");
  if (this->value == 0) {
    out->print("gpio_is_low");
  } else {
    out->print("gpio_is_high");
  }
  out->print("<div>");
  out->print(exec_button(this->name()+" 1", "Ein"));
  out->print(exec_button(this->name()+" 0", "Aus"));
  out->print(exec_button(this->name()+" pulse", "Pulse"));
  out->print("</div>");
}
void Pin::serial_print() {
  if (this->mode.equals("in")) {
    Serial.print("?name=")
    Serial.print(this->_name)
    Serial.print("&value=")
    Serial.println(this->value)
  }
}


HotPin::HotPin(int pin) {
  this->_name = "pin" + pin;
}
bool HotPin::cmd(String args[]) {
  if (this->_name.equals(args[0])) {
    if (args[1].equals("trigger")) {
      this->value = 1;
      this->lasttrigger = millis();
      digitalWrite(this->pin, 1);
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}
String HotPin::name() {
  return this->_name;
}
String HotPin::type() {
  return "HotPin";
}
void HotPin::loopActive() {
  if (since(this->lasttrigger) > this->threshold) {
    this->value = 0;
    digitalWrite(this->pin, 0);
  }
}
String HotPin::val(String name) {
  if (name.equals("mode")) {
    return "out";
  } else if (name.equals("value")) {
    return String(this->value);
  }
  return "";
}
void HotPin::page(Print* out, String param)  {
  out->print("Pin: ");
  out->print(this->pin);
  out->print("Value: ");
  out->print(this->value);
  out->print("Last Trigger: ");
  out->print(since(this->lasttrigger));
}
void HotPin::pageDetail(Print* out)  {

}


AnalogPin::AnalogPin(int pin) {
  this->pin = pin;
  this->_name = String("pin" + String(pin));
}
bool AnalogPin::cmd(String args[]) {
  if (this->_name.equals(args[0])) {
    if (args[1].equals("read")) {
      this->value = 1;
    } else if (args[1].equals("print")) {
      this->print_serial = true;
    }
    return true;
  } else {
    return false;
  }
}
String AnalogPin::name() {
  return this->_name;
}
String AnalogPin::type() {
  return "AnalogPin";
}
void AnalogPin::loopActive() {
  this->value = analogRead(this->pin);
  if (this->print_serial) {
    Serial.print(String(this->name() + ":" + String(this->value)));
  }
}
String AnalogPin::val(String name) {
  if (name.equals("value")) {
    return String(this->value);
  } 
  return "";
}
void AnalogPin::page(Print* out, String param) {
  out->print(this->name());
  out->print("Pin: ");
  out->print(this->pin);
  out->print(" Value: ");
  out->print(this->value);
  out->print(" Comment: ");
  out->print(this->comment);
}
void AnalogPin::pageDetail(Print* out)  {
  out->print("Pin: ");
  out->print(this->pin);
  out->print("<br />Value: ");
  out->print(this->value);
}
void AnalogPin::serial_print() {
  Serial.print("?name=")
  Serial.print(this->_name)
  Serial.print("&value=")
  Serial.println(this->value)
}


class NxCmds : public Item { 
  public:
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
    }
    String name() override {
        return "NxCmds";
    }
    virtual void page(Print* out, String param) override {
        out->print("NxCmds");
    }
    virtual String val(String name) override {
        return "";
    }
    virtual bool cmd(String args[]) override {
      if (args[0].equals("setTime")) {
        setTime(args[1].toInt());
      } else if (args[0].equals("gpio")) { // genauer befehl   gpio init_out 1
        add_item(new Pin(args[2].toInt(), args[1]))->activate();
      } else if (args[0].equals("hotpin")) {
        add_item(new HotPin(args[1].toInt()))->activate();
      }
      return false;
    }
};

Item* root = new NxCmds();

void items_loop(bool serial_print) {
  if (root != NULL) {
    Item* current = root;
    while (current->__next != NULL) {
      current->loop();
      if (serial_print) {
        current->serial_print();
      }
      current = current->__next;
    }
    current->loop();
  }
}

void split(String s, String res[]) {
  //String[] res = new String[10];
  int pos = 0;
  int i = s.indexOf(' ');
  while(i > -1) {
    String c = s.substring(0, i);
    c.trim();
    res[pos] = c;
    pos++;
    s = s.substring(i);
    if (s != NULL) {              // neu
      s.trim();
      i = s.indexOf(' ');
    } else {                      // neu
      i = -1;                     // neu
    }                             // neu
  }
}

boolean items_cmd(String args[]) {
  if (root != NULL) {
    Item* current = root;
    while (true) {
      if (current->cmd(args)) {
        return true;
      }
      current = current->__next;
      if (current == NULL) break;
    }  
  }
  return false;
}

Item* add_item(Item* item) {
  if (root == NULL) {
    root = item;
  } else {
    Item* current = root;
    while (current->__next != NULL) {
      current = current->__next;
    }
    current->__next = item;
  }
  item->setup();
  return item;
}

Item* item_get_root() {
  return root;
}

Item* item_get(String name) {
  Item* current = root;
  while (current->__next != NULL) {
    if (current->name().equals(name)) {
      return current;
    }
    current = current->__next;
  }
  if (current->name().equals(name)) {
      return current;
  } else {
    return NULL;
  }
}

void processResponse(const char* str) {
  //#ifdef NX_SERVER
  //  pages_response_async(str);
  //#endif
  //#ifdef NX_SERIAL
  //  #ifdef NX_SERIAL_RESPONSE
  //  Serial.println(str);
  //  #endif
  // #endif 
  String r[2] = {"response", str};
  items_cmd(r);
}


void processCommandArray(String args[], String cmd) {
  if (!items_cmd(args)) {
    if (!args[0].equals("response")) {
      processResponse(String("Error: Unknown CMD." + cmd).c_str());
    }
  } 
}

void processCommand(String cmd) {
  String args[] = {"", "", "", "", "", "", "", "", "", "", "", ""}; // 12 Elemente
  if (cmd == NULL) {
    return;
  }
  split(cmd + " ;", args);
  args[0].toLowerCase();
  processCommandArray(args, cmd);
} 
