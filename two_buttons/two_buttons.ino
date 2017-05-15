
#include "ResultBar.h"

#define BUTTONS

#define RESULTBAR_PIN     1
#define RESULTBAR_PIXELS  60
#define BUTTON1           0
#define BUTTON2           2

#define TIMEOUT           1200
#define DISPLAY_TIME      2500

#ifdef BUTTONS
#define DEBOUNCE_DELAY    2
#define MIN_TIME          5
#define MAX_TIME          200
#define ERROR_TIME        200
#else
#define DEBOUNCE_DELAY    2
#define MIN_TIME          10
#define MAX_TIME          100
#define ERROR_TIME        500
#endif

#define A (-1.0*RESULTBAR_PIXELS/(MAX_TIME - MIN_TIME))
#define B ((-A)*MAX_TIME)
#define TIME_TO_PIXELS(t) (A*t + B)

#ifdef BUTTONS
#define ENGAGED    0
#define DISENGAGED 1
#else
#define ENGAGED    1
#define DISENGAGED 0
#endif

ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);
int day_record = 0;
int all_time_record = 0;
unsigned long tdisplay = 0;


void setup() {
 pinMode(BUTTON1, INPUT_PULLUP);
 pinMode(BUTTON2, INPUT_PULLUP);
 rb.test();
}


void loop() {
  unsigned long t1;
  unsigned long t2;
  unsigned long twait;
  byte first_button;
  byte second_button;

 
  // Wait for the first button press
  while(1) {
    // Turn off bar display after predefined amount of time
    if ((tdisplay != 0) && ((millis() - tdisplay) > DISPLAY_TIME)) {
      rb.setBar(0);
      tdisplay = 0;
    }

    // Direction BUTTON1 => BUTTON2
    if (digitalRead(BUTTON1) == ENGAGED) {
      first_button = BUTTON1;
      second_button = BUTTON2;
      break;
    }

    // Direction BUTTON2 => BUTTON1
    if (digitalRead(BUTTON2) == ENGAGED) {
      first_button = BUTTON2;
      second_button = BUTTON1;
      break;
    }

  }

  // Take the first time
  t1 = millis();

#ifdef BUTTONS
  // Small delay (debouncing)
  delay(DEBOUNCE_DELAY);

  // Wait until the first button is released
  twait = millis();
  while(digitalRead(first_button) == ENGAGED)
    if ((millis() - twait) > TIMEOUT)
      return;
#endif

 
  // Wait until the second button is pressed
  twait = millis();  
  while(digitalRead(second_button) == DISENGAGED)
    if ((millis() - twait) > TIMEOUT)
      return;

  t2 = millis();
/*
  while(1) {
    rb.setBar(10);
    delay(200);
    rb.setBar(20);
    delay(200);
  }
*/
  unsigned long delta = t2 - t1;
  if (delta > ERROR_TIME) 
    rb.errorAnimation();
  else {
    rb.setBar(TIME_TO_PIXELS(delta));
    delay(1200);
    if ((day_record == 0) || (delta < day_record)) {
      day_record = delta;
      rb.newRecordAnimation();
      rb.setMark1(TIME_TO_PIXELS(day_record));
    }

    rb.setBar(TIME_TO_PIXELS(delta));
#ifdef BUTTONS
    // Wait until both buttons are depressed
    while((digitalRead(BUTTON1) == ENGAGED) || (digitalRead(BUTTON2) == ENGAGED));
#endif    
    tdisplay = millis();
  }

}

