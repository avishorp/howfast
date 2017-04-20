

#define RESULTBAR_PIN     0
#define RESULTBAR_PIXELS  60
#include "../lib/ResultBar.h"


ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);

void setup() {
}


void loop() {
    rb.test();
    delay(2000);

    rb.setMark1(20);
    rb.setMark2(50);
    for(int i = 1; i < RESULTBAR_PIXELS; i++) {
        rb.setBar(i);
        delay(50);
    }
}
