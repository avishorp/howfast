

#include <Arduino.h>
#include "../../ResultBar/lib/ResultBar.h"
#include "../../record/lib/record.h"



#define BUTTONS

#define RESULTBAR_PIN     1
#define RESULTBAR_PIXELS  57
#define BUTTON1           0
#define BUTTON2           2

#define TIMEOUT           1200
#define DISPLAY_TIME      2500

#ifdef BUTTONS
#define MIN_TIME          15
#define MAX_TIME          1000
#define ERROR_TIME        2000
#else
#define MIN_TIME          5
#define MAX_TIME          1000
#define ERROR_TIME        5000
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

 all_time_record = read_record();
}


void loop() {
  unsigned long t1;
  unsigned long t2;
  unsigned long twait;
  byte first_button;
  byte second_button;

  if (day_record > 0)
    rb.setMark1(TIME_TO_PIXELS(day_record));
  if (all_time_record > 0)
    rb.setMark2(TIME_TO_PIXELS(all_time_record));


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
  t1 = micros();


  // Wait until the second button is pressed and the first released
  twait = millis();
  while((digitalRead(second_button) == DISENGAGED) || (digitalRead(first_button) == ENGAGED))
    if ((millis() - twait) > TIMEOUT)
      return;

  t2 = micros();

  unsigned long delta = (t2 - t1)/100;
  if (delta < MIN_TIME) delta = MIN_TIME;
  if (delta > MAX_TIME) delta = MAX_TIME;
  byte bar = TIME_TO_PIXELS(delta);
  if (bar < 0) bar = 0;

  if (delta > ERROR_TIME)
    rb.errorAnimation();
  else {
    rb.setBar(bar);
    delay(1200);
    if ((all_time_record == 0) || (delta < all_time_record)) {
      // New all-time record
      all_time_record = delta;
      day_record = delta;
      write_record(delta);
      rb.newRecordAnimation();
    }
    else if ((day_record == 0) || (delta < day_record)) {
      // New day record
      day_record = delta;
      rb.newRecordAnimation();
    }

    rb.setBar(bar);
#ifdef BUTTONS
    // Wait until both buttons are depressed
    while((digitalRead(BUTTON1) == ENGAGED) || (digitalRead(BUTTON2) == ENGAGED));
#endif
    tdisplay = millis();
  }

}
