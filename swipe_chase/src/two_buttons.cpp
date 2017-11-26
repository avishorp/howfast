

#include <Arduino.h>
#include "../../ResultBar/lib/ResultBar.h"
#include "../../record/lib/record.h"


//
// Number of pixels as function of time:
//
//        |
// #PIXELS| ******
//        |       *******
//      YB|              **
//        |                **
//        |                  **
//        |                    **
//        |                      **
//        +----------------------------------------
//          MIN_        XB        MAX_
//          TIME                  TIME




#define BUTTONS

#define RESULTBAR_PIN     1
#define RESULTBAR_PIXELS  57
#define BUTTON1           0
#define BUTTON2           2

#define TIMEOUT           5000
#define DISPLAY_TIME      2500

#ifdef BUTTONS
#define MIN_TIME          15
#define MAX_TIME          2000
#define YB                (RESULTBAR_PIXELS*7/10)
#define XB                ((MAX_TIME-MIN_TIME)*6/10)
#define ERROR_TIME        4000
#else
#define MIN_TIME          5
#define MAX_TIME          1000
#define ERROR_TIME        5000
#endif


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

#define SLOPE_A ((int32_t)((int32_t)(YB - RESULTBAR_PIXELS)*1024/((int32_t)XB-(int32_t)MIN_TIME)))
#define SLOPE_B ((int32_t)(-(int32_t)YB*1024/((int32_t)MAX_TIME - (int32_t)XB)))
uint16_t time_to_pixels(uint16_t t)
{
  uint16_t x = t;
  if (t < MIN_TIME)
    x = MIN_TIME;
  if (t > MAX_TIME)
    x = MAX_TIME;

  if (x < XB)
    return ((x-MIN_TIME)*SLOPE_A/1024 + RESULTBAR_PIXELS);
  else
    return (x - XB)*SLOPE_B/1024 + YB;
}

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
    rb.setMark1(time_to_pixels(day_record));
  if (all_time_record > 0)
    rb.setMark2(time_to_pixels(all_time_record));


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
  byte bar = time_to_pixels(delta);
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
