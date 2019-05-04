#pragma once

/* PollingTimer library by dgarrett@acm.org

   This is a very light weight way to set a timer, and test to see if it
   has expired.

 */

#include "Particle.h"

class PollingTimer {
public:
  inline PollingTimer(unsigned long delay_millis, bool started=false) :
      _running(started), _delay(delay_millis) {};

  //
  // Control
  //

  // Calling start on a running timer will reset the timer.
  inline void start() {
    this->_start = millis();
    this->_running = true;
  }

  // A stopped timer will never expire or trigger an interval.
  inline void stop() {
    this->_running = false;
  }

  // Start/stop the timer based on target. Does NOT reset the timer if already
  // running.
  inline void set_running(bool target=true) {
    if (target) {
      if (!this->running())
        this->start();
    } else {
      this->stop();
    }
  }

  //
  // Status
  //

  // Is the timer current started?
  inline bool running() {
    return this->_running;
  }

  // How long as the timer been running, in millis?
  inline unsigned long waiting() {
    return this->running() ? millis() - this->_start : 0;
  }

  //
  // Results
  //

  // Returns true after the timer expires, then stops the timer. Always false
  // if the timer is stopped.
  inline bool expired() {
    bool result = this->_expired();
    if (result)
      this->stop();
    return result;
  }

  // Returns true exactly once each delay interval.
  //
  // min_delay:
  //  If true, wait for full delay before returning true again.
  //  If false, wait for delay from when previous interval expired.
  //
  // This is only relevant if you don't call interval() during the millisecond
  // in which the previous interval expired.
  inline bool interval( bool min_delay=true) {
    bool result = this->_expired();
    if (result) {
      if (min_delay)
        // Start over from right now. Re-fire after a full _delay.
        this->_start = millis();
      else
        // Start over as if we were called exactly on time.
        this->_start += this->_delay;
    }
    return result;
  }

private:
  inline bool _expired() {
    // This ordering is int overflow safe, as long as the actual time since
    // start isn't > ULONG_MAX (~50 days).
    return this->running() && (this->waiting() >= this->_delay);
  }

  bool _running;
  unsigned long _start;
  unsigned long _delay;
};


// Debouncer makes inconsistent input values more consistent.
//   debounce_time: How long does an input need to be consistent to be trusted?
//                  Used for both false -> true and true -> false.
//   initial: What is the initial value?
class Debouncer {
public:
  inline Debouncer(unsigned long debounce_time=10, bool initial=false) :
      _value(initial), _timer(debounce_time) {};

  // Update the debouncer with a new input value.
  //   input: Is the value the the value to debounce. A GPIO input, etc.
  //
  // Return: true when value() transitions in either direction.
  inline bool changed(bool input) {
    // Ensure the timer is running if input != to current value.
    this->_timer.set_running(input != this->value());

    if (this->_timer.expired()) {
      this->_value = input;
      return true;
    }

    return false;
  }

  // Update the debouncer with a new input value.
  //   input: Is the value the the value to debounce. A GPIO input, etc.
  //
  // Return: true when value() transitions from false to true.
  //         Useful for trigging an action on button press, etc.
  inline bool debounce(bool input) {
    if (this->changed(input))
      return this->_value;

    return false;
  }

  // The current debounced value.
  inline bool value() {
    return this->_value;
  }

  // We are in the middle of a potential value change, so the current value
  // is "soft".
  inline bool soft() {
    return this->_timer.running();
  }

private:
  bool _value;
  PollingTimer _timer;
};
