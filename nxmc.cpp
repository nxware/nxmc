
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