
#include "PollingTimer.h"

// Hook up an button between 3.3v and D0.
//
// Tap the button, and nothing happens. Hold the button, and the D7 led
// stays on for as long as you hold it. You get a serial console message
// when pushing the button, but it doesn't repeat until you let off and
// then hold the button again.

const unsigned long MILLIS = 2000; // Long debounce to make it easier to see.

Debouncer debouncer(MILLIS);

void setup() {
  pinMode(D0, INPUT_PULLDOWN);
  pinMode(D7, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (debouncer.debounce(digitalRead(D0))) {
    Serial.println("D0 active!");
  }
  digitalWrite(D7, debouncer.value());
}
