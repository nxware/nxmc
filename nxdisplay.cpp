#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <nxmc.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 hw_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_value(float value, int size, int x, int y) {
  char _str[10];
  hw_display.setTextSize(size);      // Normal 1:1 pixel scale
  hw_display.setTextColor(SSD1306_WHITE); // Draw white text
  hw_display.setCursor(x, y);     // Start at top-left corner
  hw_display.cp437(true);         // Use full 256 char 'Code Page 437' font
  dtostrf( value, 6, 2, _str );
  hw_display.write(_str);
}

void display_str(String value, int size, int x, int y) {
  hw_display.setTextSize(size);      // Normal 1:1 pixel scale
  hw_display.setTextColor(SSD1306_WHITE); // Draw white text
  hw_display.setCursor(x, y);     // Start at top-left corner
  hw_display.cp437(true);         // Use full 256 char 'Code Page 437' font
  hw_display.write(value.c_str());
}

void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  hw_display.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
}

void display_display() {
    hw_display.display();
}

class DisplayItem : public Item { 
  public:
    int size = 1;
    int x = 0;
    int y = 0;
    String item_name;
    String value_name;
    DisplayItem(int size, int x, int y, String item_name, String value_name) {
        this->size = size;
        this->x = x;
        this->y = y;
        this->item_name = item_name;
        this->value_name = value_name;
    }
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
        
    }
    void display() override {
        String s = item_get(this->item_name)->val(this->value_name);
        display_str(s, this->size, this->x, this->x);
    }
    String name() override {
        return this->item_name + "_" + this->value_name;
    }
    virtual void page(Print* out, String param) override {
        out->print("NxDisplay");
    }
    virtual String val(String name) override {
        return "";
    }
    virtual bool cmd(String args[]) override {
      return false;
    }
};

void display_add(int size, int x, int y, String item_name, String value_name) {
  Item* itm = new DisplayItem(size, x, y, item_name, value_name);
  itm->activate();
  add_item(itm);
}

class NxDisplay : public Item { 
  public:
    int start_delay = 0;
    int alert = 0;
    String alert_str = "";
    NxDisplay(int start_delay = 0) {
        this->start_delay = start_delay;
    }
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
        hw_display.clearDisplay();
        if (this->alert > 0) {
            display_str(this->alert_str, 1, 5, 10);
            this->alert = this->alert - 1;
        } else if (start_delay*1000 < millis()) {
            Item* current = item_get_root();
            while (current->__next != NULL) {
                current->display();
                current = current->__next;
            }
            current->display();
        } else {
            // TODO besser draw line
            display_value((int)(millis()-start_delay*1000)/1000, 1, 5, 5); // läuft von -3 bis 0
        }
        hw_display.display();
    }
    String name() override {
        return "nxdisplay";
    }
    virtual void page(Print* out, String param) override {
        out->print("NxDisplay");
    }
    virtual String val(String name) override {
        return "";
    }
    virtual bool cmd(String args[]) override {
      if (args[0].equals(this->name()) && args[1].equals("alert")) {
          this->alert_str = args[2];
          this->alert = 10;
          return true;
      } else if (args[0].equals(this->name()) && args[1].equals("add")) {
        display_add(args[2].toInt(), args[3].toInt(), args[4].toInt(), args[5], args[6]);
      } else {
        if (args[0].equals(this->name())) {
          if (args[1].equals("rotate")) {
            hw_display.setRotation(args[2].toInt());
          }
        }
      }
      return false;
    }
};


void display_start(int start_delay = 0) {

  if (item_get("nxdisplay")==NULL) {
    NxDisplay* item = new NxDisplay(start_delay);
    item->activate();
    add_item(item);
  }

  if(!hw_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  } // https://learn.adafruit.com/monochrome-oled-breakouts/wiring-128x64-oleds

  hw_display.setRotation(2);
  hw_display.clearDisplay();

  display_str("start", 2, 5, 15);  
  display_display();
}

void display_clear() {
    hw_display.clearDisplay();
}

void display_status(String value) {
  display_clear();
  display_str(value,1,4,4);
  display_display();
}
