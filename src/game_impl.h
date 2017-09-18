#ifndef GAME_IMPL_H
#define GAME_IMPL_H

#include "card.h"
#include "game.h"

class GameImpl : public Game {
  map<Event::EventType, vector<Action*>> listeners_;
  Field field_;

 public:
  bool AddEventListener(Event::EventType event_type, Action* action);

  Field& GetField();

  // (TODO) Implement this function.
  string GetEnemyId(const string& me);
};
#endif