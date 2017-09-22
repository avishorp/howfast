
#include <Arduino.h>
#include "../lib/record.h"
#include "../../ResultBar/lib/ResultBar.h"

#define RESULTBAR_PIN    1
#define RESULTBAR_PIXELS 60

ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);

void setup()
{
  erase_record();
}

void loop()
{
  rb.setBar(10);
  delay(300);
  rb.setBar(20);
  delay(300);
}
