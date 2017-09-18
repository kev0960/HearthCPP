#ifndef CARD_ACTION_H
#define CARD_ACTION_H

#include "card.h"
#include "conditional.h"
#include "logging.h"
#include "target.h"

/*

Action := To(target).Do(Action)
  Anything on same "Do" shares the target. If you want to set up the different
  targets, then separate it from using Then or And.
Action := Action.And(Action)
  Actions that are connected through "And" happens together. That means, the
  event handler will not be called during processing actions.
Action := Action.Then(Action)
  Actions that are connected through "Then" does not happen together. The action
  that comes in "Then" will be executed AFTER the previous events have
processed.
Action := If(Conditional).Then(Action when true, Action when false)

Examples.
  To(ALL_MINIONS).Do(DESTROY).And(To(MY_CARDS).Then(DISCARD));
  To(USER_TARGET).Do(If(TARGET is good).Then(Destroy(TARGET)));
  To(USER_TARGET).Do(Hit(3).And(Freeze(1)));

*/
class Action;

class CardAction {
  // ID of the card who owns this action.
  string card_id_;

  Action* on_play;
  Action* on_hand;
  Action* on_deck;

 public:
  CardAction() {}
  CardAction(const string& card_id) : card_id_(card_id) {}

  virtual bool DoAction() = 0;
};

class CardActionManager {
  // card_id to action map
  map<string, CardAction*> card_actions_list_;
  CardActionManager() {}

 public:
  static CardActionManager& GetCardManager() {
    static CardActionManager card_manager;
    return card_manager;
  }

  // Returns true if the card action is successfully registered.
  bool RegisterCardAction(const string& card_id, CardAction* card_action) {
    if (card_actions_list_.find(card_id) == card_actions_list_.end()) {
      card_actions_list_[card_id] = card_action;
      return true;
    }
    return false;
  }
};

// Examples
// To(ALL_MINIONS).Do(DESTROY(ALL_MINIONS)).Then(
//  To.(MY_CARDS).Do(DISCARD(MY_CARDS)))  (Deathwing)
class Action {
 protected:
  vector<vector<Action*>> action_list_;
  int current_pointer_;
  Target* target_;

 public:
  Action() : current_pointer_(-1), target_(nullptr) {}

  // Two actions are done together
  virtual Action& And(const Action& action) {
    // If action_list is empty, create the first bin.
    if (action_list_.empty()) {
      action_list_.push_back(vector<Action*>());
      current_pointer_ = 0;
    }
    action_list_.resize(action_list_.size() + action.action_list_.size() - 1);
    for (size_t i = 0; i < action.action_list_.size(); i++) {
      action_list_.at(current_pointer_)
          .insert(action_list_.at(current_pointer_).end(),  // position
                  action.action_list_.at(i).begin(),        // start
                  action.action_list_.at(i).end()           // end
                  );
      current_pointer_++;
    }
    return *this;
  }

  // Adds an action that happens next.
  virtual Action& Then(const Action& action) {
    action_list_.resize(action_list_.size() + action.action_list_.size());
    current_pointer_++;

    for (size_t i = 0; i < action.action_list_.size(); i++) {
      action_list_.at(current_pointer_) = action.action_list_[i];
      current_pointer_++;
    }

    return *this;
  }

  virtual DoAction() = 0;
  virtual ~Action() {}
};

// 'To' directive directs the targets to conduct "Do" things. Note that once the
// target is found, the target will be passed as an information to the entire
// 'Action' clause. If there is the nested "To..Do" clause inside of Actions,
// then
// the closest To..Do clause will be chosen.
// USAGE : To(Targets).Do(Actions)
class To : public Action {
 public:
  To(Target& target) {
    if (target_ != nullptr) {
      return;
    }
    target_ = new Target(target);
  }

  // Every Actions that belongs in same "Do" shares the target.
  // This means that if you want to delegate the targets accross the multiple
  // actions, then you must put it in same "Do"
  Action& Do(const Action& action) {
    // Non empty action_list_ implies that Do function has called more than
    // once.
    assert(action_list_.empty());

    action_list_ = action.action_list_;
    for (auto bin = action_list_.begin(); bin != action_list_.end(); bin++) {
      for (auto itr = bin->begin(); itr != bin->end(); itr++) {
        if ((*itr)->target_ != nullptr) (*itr)->target_ = target_;
      }
    }
  }

  ~To() {
    if (target_ != nullptr) {
      delete target_;
    }
  }
};

class If : public Action {
  Conditional cond_;

 public:
  If(const Conditional& cond) : cond_(cond) {}

  bool Check() { return cond_.Check(); }
}

class HIT : public Action {
 public:
};

#endif