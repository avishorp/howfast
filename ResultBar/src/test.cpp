

#define RESULTBAR_PIN     0
#define RESULTBAR_PIXELS  60
#include "../lib/ResultBar.h"


ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);

void setup() {
}


void loop() {
    rb.test();
    delay(5000);
}
