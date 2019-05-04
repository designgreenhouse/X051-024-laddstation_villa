
#include "PollingTimer.h"

// Turn off the D7 LED after MILLIS delay.
const unsigned long MILLIS = 2000;

PollingTimer timer(MILLIS);

void setup() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 1);

  timer.start();
}

void loop() {
  if (timer.expired())
    digitalWrite(D7, 0);
}
