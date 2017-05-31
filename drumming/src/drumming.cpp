// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Arduino.h>
#include "LIS3DH_small.h"
#include <Adafruit_Sensor.h>
#include "../../ResultBar/lib/ResultBar.h"
#include "../../record/lib/record.h"

//#define TRINKET_PRO
//#define PULSE_LED

#ifdef TRINKET_PRO
#define RESULTBAR_PIN 4
#define INTERNAL_LED 13
#else
#define RESULTBAR_PIN 1
#define INTERNAL_LED 1
#endif
#define RESULTBAR_PIXELS  60


#define WINDOW_SIZE 16
#define THRESHOLD_LOW (-17000)
#define UPDATE_PERIOD 400
#define HIT_WAIT 40
#define MAX_RATE 800
#define HIT_TIMEOUT 3000

#define LED_PULSE_LENGTH 100
#define MIN_COUNT_TO_DISPLAY 7

#define RATE_TO_PIXELS(r) ((unsigned int)RESULTBAR_PIXELS*(unsigned int)r/MAX_RATE)

ResultBar rb(RESULTBAR_PIXELS, RESULTBAR_PIN);

// I2C
Adafruit_LIS3DH_small lis = Adafruit_LIS3DH_small();
int day_record = 0;
int all_time_record = 0;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

void setup(void) {
#ifdef TRINKET_PRO
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  Serial.begin(9600);
#endif


  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
#ifdef TRINKET_PRO
    Serial.println("Couldnt start");
#endif
    while (1);
  }
  //Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

#ifdef TRINKET_PRO
  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");
#endif
  rb.test();

#ifdef PULSE_LED
  pinMode(INTERNAL_LED, OUTPUT);
#endif
  // Fetch all-time record from the EEPROM
  all_time_record = read_record();
  if (all_time_record > 0) {
    rb.setMark2(RATE_TO_PIXELS(all_time_record));

#ifdef TRINKET_PRO
    Serial.print("all time record ");
    Serial.println(all_time_record);
    Serial.println(RATE_TO_PIXELS(all_time_record));
#endif
  }

}


void loop() {
  unsigned long hits[WINDOW_SIZE];
  byte head = 0;
  byte count = 0;
  unsigned long next_update = millis() + UPDATE_PERIOD;
  unsigned long last_hit = 0;
  int current;
  unsigned long previous;
  unsigned long led_pulse_time;
  uint16_t max_rate = 0;
  uint16_t display_max_rate = 0;
  uint16_t bar;

  // Clear the bar display
  rb.setBar(0);


  while(1) {
    // Measure acceleration, store the data
    lis.read();
    unsigned long now = millis();
    unsigned long last_update = now;
    previous = current;
    current = lis.z;

    if ((now-last_hit > HIT_WAIT) && (lis.z < THRESHOLD_LOW)) {
    //if ((now-last_hit > HIT_WAIT) && (current > THRESHOLD_HIGH) && (previous < THRESHOLD_LOW)) {
      // A hit was detected!

      last_hit = now;

      // Push the new measurement into the head of the cyclic buffer
      hits[head] = now;

      // Keep track of the number of the samples in the buffer
      if (count < WINDOW_SIZE) {
        count++;
      }

      // Increment the head
      head = (head + 1) % WINDOW_SIZE;
      //Serial.print("count "); Serial.print(count); Serial.print(' '); Serial.println(now);

      // Flash the internal led
#ifdef PULSE_LED
      digitalWrite(INTERNAL_LED, HIGH);
      led_pulse_time = 0;
#endif

    }

    if (millis() > next_update) {
      // Update the output

      // First, check if there are enough measurments
      if (count > MIN_COUNT_TO_DISPLAY) {
        // Find the times of the first and last recorded hits
        unsigned long latest = hits[(head - 1 + count) % WINDOW_SIZE];
        unsigned long first = hits[head];
        unsigned long delta_t = latest - first;

        // Calculate the rate in BPM x10
//        float rate = 1000.0*60*count / delta_t;
        int rate = (600*count / delta_t)*100;;
        bar = RATE_TO_PIXELS(rate);

        // Calc the max ratte
        if ((count == WINDOW_SIZE) && (rate > max_rate))
          max_rate = rate;

        // Schedule the next update
        next_update = millis() + UPDATE_PERIOD;

        //Serial.println(rate);
        //Serial.println((int)(RESULTBAR_PIXELS*rate/800));

        rb.setBar(bar);
      }
    }

    // Check if a timeout has elapsed since the last hit, reset if so
    if ((now - last_hit) > HIT_TIMEOUT) {
      // Check if a new record has been maid
      if (count == WINDOW_SIZE) {
        if (max_rate > all_time_record) {
          // New all time record!
          all_time_record = max_rate;
          day_record = max_rate;
          write_record(max_rate);
          rb.newRecordAnimation();

          rb.setMark1(bar);
          rb.setMark2(bar);
        }
        else if (max_rate > day_record) {
          // New day record!
          day_record = max_rate;
          rb.newRecordAnimation();
          rb.setMark1(bar);
        }
      }
#ifdef PULSE_LED
      digitalWrite(INTERNAL_LED, 0);
#endif
      return;
    }


#ifdef PULSE_LED
    if ((now - led_pulse_time) > LED_PULSE_LENGTH) {
      digitalWrite(INTERNAL_LED, 0);
      led_pulse_time = 99999999999;
    }
#endif
  }
}
