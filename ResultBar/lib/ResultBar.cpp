
#include "ResultBar.h"

#define TEST_BRIGHTNESS 10
#define TEST_DELAY      300

// Colors of the different elements
#define COLOR_MARK1    (Adafruit_NeoPixel::Color(200, 0, 0))
#define COLOR_MARK2    (Adafruit_NeoPixel::Color(200, 0, 100))
#define COLOR_BAR      (Adafruit_NeoPixel::Color(0, 100, 0))

void ResultBar::test()
{
    // Red
    pixels.clear();
    setAll(pixels.Color(TEST_BRIGHTNESS, 0, 0));
    pixels.show();
    delay(TEST_DELAY);

    // Green
    pixels.clear();
    setAll(pixels.Color(0, TEST_BRIGHTNESS, 0));
    pixels.show();
    delay(TEST_DELAY);

    // Blue
    pixels.clear();
    setAll(pixels.Color(0, 0, TEST_BRIGHTNESS));
    pixels.show();
    delay(TEST_DELAY);

    setBar(0);
}

void ResultBar::setMark1(uint8_t value)
{
    if (value <= pixels.numPixels())
        mark1 = value;
    else
        mark1 = 0;
}

void ResultBar::setMark2(uint8_t value)
{
    if (value <= pixels.numPixels())
        mark2 = value;
    else
        mark2 = 0;
}

void ResultBar::setBar(uint8_t value)
{
    // First, clear all
    pixels.clear();

    // Draw the bar
    if ((value > 0) &&  (value <= pixels.numPixels())) {
        for(uint8_t i = 1; i <= value; i++)
            pixels.setPixelColor(i-1, COLOR_BAR);
    }

    // Draw marks
    if (mark1 > 0)
        pixels.setPixelColor(mark1-1, COLOR_MARK1);
    if (mark2 > 0)
        pixels.setPixelColor(mark2-1, COLOR_MARK2);

    pixels.show();


}

void ResultBar::newRecordAnimation()
{
    uint8_t temp[5];
    uint8_t n;
    uint8_t r, g, b;
    uint8_t bright = 50;
    uint8_t sat = 250;
    uint8_t center = pixels.numPixels() / 2;

    pixels.clear();

    // HLS to RGB Algorithm based on https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
    for(int index=0; index < 2000; index+=10) {
      // Convert the hue (index) to RGB values
      n = (index >> 8) % 3;
      // %3 not needed if input is constrained, but may be useful for color cycling and/or if modulo constant is fast
      uint8_t x = ((((index & 255) * sat) >> 8) * bright) >> 8;
      // shifts may be added for added speed and precision at the end if fast 32 bit calculation is available
      uint8_t s = ((256 - sat) * bright) >> 8;
      temp[0] = temp[3] = s;
      temp[1] = temp[4] = x + s;
      temp[2] = bright - x    ;
    
      r = temp[n + 2];
      g = temp[n + 1];
      b = temp[n];

      // Propagate the center color outwards
      for(int8_t p = 0; p < center; p++) {
        uint32_t c = pixels.getPixelColor(p+1);
        pixels.setPixelColor(p, c);
        pixels.setPixelColor(pixels.numPixels() - p - 1, c);
      }

      // Set the center color
      pixels.setPixelColor(center, Adafruit_NeoPixel::Color(r, g, b));

      // Show the result for a short while
      pixels.show();
      delay(15);
    }
    
}

void ResultBar::setAll(uint32_t color)
{
    for(uint8_t i=0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, color);
    }
}
