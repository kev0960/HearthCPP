#include "game_impl.h"

bool GameImpl::AddEventListener(Event::EventType event_type, Action* action) {
  listeners_[event_type].push_back(action);
  return true;
}

Field& GameImpl::GetField() { return field_; }

// (TODO) Implement this function.
string GameImpl::GetEnemyId(const string& me) { return ""; }
