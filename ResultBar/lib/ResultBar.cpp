
#include "ResultBar.h"

#define TEST_BRIGHTNESS 10
#define TEST_DELAY      300


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

    pixels.clear();
    pixels.show();
}

void ResultBar::setMark1(int value)
{

}

void ResultBar::setMark2(int value)
{

}

void ResultBar::setBar(int value)
{

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
