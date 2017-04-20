
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

class ResultBar {

public:
    ResultBar(int numPixels, int pin) : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800) { 
#if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif        
        pixels.begin();
    }

    // Display a short test sequence
    void test();

    // Sets the all-time record value. It will be displayed in red
    void setMark1(int value);

    // Sets the session record value.
    void setMark2(int value);

    // Set the value of the bar
    void setBar(int value);

    // Start new record animation
    void newRecordAnimation();

protected:
    void setAll(uint32_t color);

    Adafruit_NeoPixel pixels; 


};