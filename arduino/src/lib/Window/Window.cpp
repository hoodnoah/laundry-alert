#include "Window.h"
#include "../Constants/Constants.h"

Window Window_new(int window_size)
{
  return Window(0, window_size, 0);
}

// adds a new reading to the window.
// loops back around when the number of readings has been met/exceeded
void Window_AddReading(Window &window, bool active)
{
  // save value of old reading before replacement
  bool oldReading = window.readings[window.index];

  // set the current index, increment to next spot
  window.readings.set(window.index, active);
  window.index++;

  // loop back to start; this permits overwriting the oldest value when we run out of space
  if (window.index >= window.window_size)
  {
    window.index = 0;
  }

  // adjust the proportion for removing the old value, adding the new value
  if (oldReading)
  {
    window.countAboveThreshold--;
  }

  if (active)
  {
    window.countAboveThreshold++;
  }
}

// returns true when enough observations in the sample window
// are positive
bool Window_IsActive(Window &window)
{
  return (window.countAboveThreshold >= NUM_ACTIVE);
}