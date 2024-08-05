#ifndef WINDOW_H
#define WINDOW_H

#include "../Constants/Constants.h"
#include "../Accelerometer/Accelerometer.h"

#include <bitset>

struct Window
{
  std::bitset<SAMPLE_WINDOW_SIZE> readings;
  int window_size;
  int index;
  int countAboveThreshold;

  Window(int index = 0, int window_size = SAMPLE_WINDOW_SIZE, int countAboveThreshold = 0) : index(index), window_size(window_size), countAboveThreshold(countAboveThreshold) {};
};

Window Window_new(int windowSize);
void Window_AddReading(Window &window, bool active);
bool Window_IsActive(Window &window);

#endif