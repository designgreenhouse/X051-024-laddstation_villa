
#include "PollingTimer.h"

// If the digital read from pin D0 is true for >= 50 milliseconds, turn
// on pin D7. If D0 reads false, turn off D7.

const unsigned long MILLIS = 1000;

PollingTimer timer(MILLIS);

void setup() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 0);

  // Start/Stop on the timer will start/stop the intervals.
  timer.start();
}

bool led = false;

void loop() {
  if (timer.interval()) {
    led = !led;
    digitalWrite(D7, led);
  }
}
