
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

class ResultBar {

public:
    ResultBar(int numPixels, int pin) :
        pixels(numPixels, pin, NEO_GRB + NEO_KHZ800),
        mark1(0),
        mark2(0) {
#if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
        pixels.begin();
    }

    // Display a short test sequence
    void test();

    // Sets the all-time record value. It will be displayed in red
    void setMark1(uint8_t value);

    // Sets the session record value.
    void setMark2(uint8_t value);

    // Set the value of the bar
    void setBar(uint8_t value);

    // Start new record animation
    void newRecordAnimation();

	// Error animation
	void errorAnimation();

protected:
    void setAll(uint32_t color);
    void render();

    Adafruit_NeoPixel pixels;
    int mark1, mark2;
    int barValue;


};
