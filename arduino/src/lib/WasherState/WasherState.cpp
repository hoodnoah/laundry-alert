#include "WasherState.h"

// constructor
WasherState::WasherState(unsigned long updateCoolDownMs)
{
  this->updateCoolDownMs = updateCoolDownMs;
  this->state = Activity::IDLE;
  this->lastStateUpdateMs = 0;
}

// returns the current state
Activity WasherState::getState()
{
  return this->state;
}

// permits a change to the state if the cooldown is expired
bool WasherState::trySetState(Activity newState)
{
  Activity oldState = this->state;

  if (true == this->coolDownExpired())
  {
    this->state = newState;
  }

  return oldState == this->state;
}

// attempts to set state active
// resets cooldown on change/setting active while active
bool WasherState::setActive()
{
  bool changed = this->trySetState(Activity::ACTIVE);

  // reset the cooldown if we are active at method end
  if (Activity::ACTIVE == this->state)
  {
    this->resetLastUpdateMs();
  }

  return changed;
}

// attempts to set state idle
// only succeeds if cooldown is expired
// resets the cooldown on change, or on update when state
// already idle.
bool WasherState::setIdle()
{
  bool changed = this->trySetState(Activity::IDLE);

  // reset the cooldown if idle at method end
  if (Activity::IDLE == this->state)
  {
    this->resetLastUpdateMs();
  }

  return changed;
}

// determines whether or not the cooldown has expired
bool WasherState::coolDownExpired()
{
  return (millis() - this->lastStateUpdateMs) >= this->updateCoolDownMs;
}

// resets the last time the state was updated
void WasherState::resetLastUpdateMs()
{
  this->lastStateUpdateMs = millis();
}