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

class NxDisplay : public Item { 
  public:
    int start_delay = 0;
    NxDisplay(int start_delay = 0) {
        this->start_delay = start_delay;
    }
    void init() {}
    virtual void setup() override {}
    void loopActive() override {
        hw_display.clearDisplay();
        if (start_delay*1000 < millis()) {
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
        return "NxDisplay";
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


void display_start(int start_delay = 0) {
  NxDisplay* item = new NxDisplay(start_delay);
  item->activate();
  add_item(item);

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
