
#include <Arduino.h>

#include <nxmc.h>

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

Item* root = NULL;

void items_loop() {
  if (root != NULL) {
    Item* current = root;
    while (current->__next != NULL) {
      current->loop();
      current = current->__next;
    }
    current->loop();
  }
}

extern void split(String s, String res[]) {
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
