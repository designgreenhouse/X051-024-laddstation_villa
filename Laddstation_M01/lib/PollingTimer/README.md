# PollingTimer

A Particle library for PollingTimer

## Usage

Simple timer to track how much time has passed in milliseconds that is
overflow safe. It has start/stop/expired support, interval alert support, and
easy debounce support.

This library should work with any Arduino, but the examples are written
for the Particle Photon.
```
#include "PollingTimer.h"

PollingTimer timer(100);
PollingTimer interval(50);
Debouncer debounce(10);

void setup() {
  pinMode(D0, INPUT_PULLDOWN);

  timer.start();
  interval.start();
}

void loop() {
  if (timer.expired())
    Serial.print("100 milliseconds have passed!");

  if (interval.interval())
    Serial.print("50 milliseconds, repeating.");

  if (debounce.debounce(digitalRead(D0)))
    Serial.print("Button pushed.");
}
```

See the [examples](examples) folder for more details.

## Documentation

TODO: Describe `PollingTimer`

## Contributing

It's available at [github.com/DonGar/PollingTimer](https://github.com/DonGar/PollingTimer).

Please feel free to send me pull requests.

## LICENSE
Copyright 2017 dgarrett@acm.org

Licensed under the Apache 2 license.
