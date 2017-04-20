
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
    
}

void ResultBar::setAll(uint32_t color)
{
    for(uint8_t i=0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, color);
    }
}
