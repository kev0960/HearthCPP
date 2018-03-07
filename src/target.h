#ifndef TARGET_H
#define TARGET_H

#include <algorithm>
#include "card.h"
#include "game.h"
/*
  How Target Works.

  Target class is passed to the any Action object that requires the target. For
  example, Hit object wants to know what cards it must target. Also when the
  user plays the card,
  sometimes it requires the target to do something (like giving buffs or deals
  damage.)

  In all cases, we need to pass Target object to tell what kind of actions
  it needs to do.

  Sometimes the target requires the user interaction (by clicking the target).
  In
  this case, we have to pass UserTarget object.

*/
class Target {
  Game* game_;

  // Owner of this Target.
  Card* card_;

  vector<Card*> targets_;

  bool targets_found_;
  // bool (const Game* game, Card* my_card, Card* card_need_to_be_checked)
  vector<function<bool(Game*, Card*, Card*)>> add_targets_fn_;
  vector<function<bool(Game*, Card*, Card*)>> exclude_targets_fn_;

 public:
  Target(Game* game);
  Target(const function<bool(Game*, Card*, Card*)> add_target);
  Target(Game* game, Card* card);
  Target(const Target& target);

  virtual bool FindTargets(Card* card);
  virtual vector<Card*> GetTargets();

  Target& operator+(const Target& target);

  Target& operator-(const Target& target);

  void SetGame(Game* game) { game_ = game; }
  void SetCard(Card* card) { card_ = card; }

  static Target ALL_MY_MINIONS;
  static Target ALL_MY_CHARACTERS;
  static Target ALL_ENEMY_CHARACTERS;
  static Target ALL_ENEMY_MINIONS;

  virtual ~Target() {}
};

class UserTarget : public Target {
 public:
  UserTarget(Game* game) : Target(game) {}
  UserTarget(const function<bool(Game*, Card*, Card*)> target_candidate);
};

#endif