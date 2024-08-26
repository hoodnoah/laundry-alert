#ifndef WASHERSTATE_H
#define WASHERSTATE_H

enum Activity
{
  ACTIVE,
  IDLE
};

class WasherState
{
public:
  WasherState(unsigned long updateCoolDownMs);
  // getters
  Activity getState();

  // setters
  bool setActive();
  bool setIdle();

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