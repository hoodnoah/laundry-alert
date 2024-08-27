#ifndef WASHERSTATE_H
#define WASHERSTATE_H

#include <Arduino.h>

enum Activity
{
  ACTIVE,
  IDLE
};

class WasherState : public Printable
{
public:
  WasherState(unsigned long updateCoolDownMs);
  // getters
  Activity getState();

  // setters
  bool setActive();
  bool setIdle();

  // overrides
  size_t printTo(Print &p) const;

private:
  // vars
  Activity state;
  unsigned long updateCoolDownMs;
  unsigned long lastStateUpdateMs;

  // methods
  bool coolDownExpired();
  void resetLastUpdateMs();
  bool trySetState(Activity newState);
};

#endif