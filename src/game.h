#ifndef GAME_H
#define GAME_H
#include "card.h"
class Action;

class Game {
 public:
  virtual bool AddEventListener(Event::EventType event_type,
                                Action* action) = 0;
  virtual Field& GetField() = 0;
  virtual string GetEnemyId(const string& me) = 0;
};

#endif