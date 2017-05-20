
#include <Arduino.h>
#include "../../ResultBar/lib/ResultBar.h"
#include "../../record/lib/record.h"

#define LED         2
#define BUTTON      0

#define RESULTBAR_PIN     1
#define RESULTBAR_PIXELS  57

#define DISPLAY_TIME      200
#define TIMEOUT           2000
#define MIN_TIME          10
#define MAX_TIME          500

#define A (-1.0*RESULTBAR_PIXELS/(MAX_TIME - MIN_TIME))
#define B ((-A)*MAX_TIME)
#define TIME_TO_PIXELS(t) (A*t + B)

ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);
int day_record = 0;
int all_time_record = 0;


void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  pinMode(BUTTON, INPUT_PULLUP);
  rb.test();

  if (day_record > 0)
    rb.setMark1(TIME_TO_PIXELS(day_record));
  if (all_time_record > 0)
    rb.setMark2(TIME_TO_PIXELS(all_time_record));

}

void loop()
{
  unsigned long tnow;

  rb.setBar(0);

  // Add a small random delay
  delay(random(10, 1000));

  // Turn on the LED
  unsigned long t0 = millis();
  digitalWrite(LED, 0);

  while(1) {
    tnow = millis();

    if (tnow - t0 > DISPLAY_TIME)
      digitalWrite(LED, 1);

    if (tnow - t0 > TIMEOUT)
      // Restart
      return;

    if (digitalRead(BUTTON) == 0)
      // Buton pressed
      break;
  }

  // Calculate the resonse time, then show it
  unsigned long tresponse = tnow - t0;
  if (tresponse > MAX_TIME) {
    rb.errorAnimation();
  }
  else {
    digitalWrite(LED, 1);
    rb.setBar(TIME_TO_PIXELS(tresponse));

    if ((day_record == 0) || (tresponse < day_record)) {
      // New day record
      delay(800);
      rb.newRecordAnimation();

      day_record = tresponse;
      rb.setMark1(TIME_TO_PIXELS(tresponse));
    }
    else
      delay(1500);
  }

  // Make sure the button is released
  while(digitalRead(BUTTON) == 0);
}
