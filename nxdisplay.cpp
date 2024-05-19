#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void display_start() {

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  } // https://learn.adafruit.com/monochrome-oled-breakouts/wiring-128x64-oleds

  display.setRotation(2);
  display.clearDisplay();
}

void display_display() {
    display.display();
}

void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  display.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
}



void display_value(float value, int size, int x, int y) {
  char _str[10];
  display.setTextSize(size);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  dtostrf( value, 6, 2, _str );
  display.write(_str);
}

void display_str(String value, int size, int x, int y) {
  display.setTextSize(size);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write(value.c_str());
}