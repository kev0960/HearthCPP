#include <memory>
#include "hscard_db.h"

class CardAction;
class UniqueId {
  int current_;
  UniqueId() : current_(0) {}

 public:
  static UniqueId& GetUniqueIdGen() {
    static UniqueId unique_id;
    return unique_id;
  }

  string GetUniqueId() {
    current_++;
    return to_string(current_);
  }

  UniqueId(const UniqueId& u) = delete;
  void operator=(const UniqueId& u) = delete;
};

class Event {
 public:
  enum EventType {
    DRAW_CARD,  // From the Deck
    PLAY_CARD,  // From the Hand
    DESTROY,
    AFTER_PLAY,
    SUMMON,
    // Minions
    PROPOSE_ATTACK,
    ATTACK,
    AFTER_ATTACK,
    HERO_ATTACK
  };

  Event(EventType type) : type_(type) {}

 private:
  EventType type_;
};

// Adopted from Hearthstone Wiki.
enum Zone {
  PLAY,  // Game field
  DECK,
  HAND,
  SECRET,
  GRAVEYARD,
  REMOVED_FROM_GAME,
  SET_ASIDE
};

// The instantiated card which exists in Game.
class Card {
  // Unique ID of the card which is assigned every time the card
  // is generated.
  string uid_;
  CardInfo card_info_;
  string owner_id_;

  // Where the card is?
  Zone zone_;

 public:
  // Builds a Card from the card ID.
  Card(const string& card_id, const string& owner_id) : owner_id_(owner_id) {
    UniqueId& unique_gen = UniqueId::GetUniqueIdGen();
    uid_ = unique_gen.GetUniqueId();

    HSCardDB& card_db = HSCardDB::GetHSCardDB();
    card_info_ = card_db.Search(card_id);
  }

  string GetOwnerId() { return owner_id_; }
  Zone GetZone() { return zone_; }
};

class Hero : public Card {};

class Field {
  vector<Card*> cards_;

 public:
  vector<Card*> FindPlayerCard() { return cards_; }

  vector<Card*> FindPlayerCard(const string& owner_id) {
    return FindPlayerCard(owner_id);
  }
  vector<Card*> FindPlayerCard(Zone card_location) {
    return FindPlayerCard("ANY", card_location);
  }
  vector<Card*> FindPlayerCard(const string& owner_id,
                               Zone card_location = -1) {
    vector<Card*> my_cards;
    for (auto itr = cards_.begin(); itr != cards_.end(); itr++) {
      if ((*itr)_ > GetOwnerId() == owner_id || owner_id == "ANY") {
        if (card_location == -1)
          my_cards.push_back(*itr);
        else if ((*itr)->GetZone() == card_location)
          my_cards.push_back(*itr);
      }
    }

    return my_cards;
  }
};

class Game {
  map<Event::EventType, vector<Action>> listeners_;
  Field field_;

 public:
  bool AddEventListener(Event::EventType event_type, const Action& action) {
    listeners_[event_type].push_back(action);
    return true;
  }

  Field& GetField() { return field_; }

  // (TODO) Implement this function.
  string GetEnemyId(const string& me) { return ""; }
};

class CardActionManager {
  // card_id to action map
  map<string, CardAction> card_actions_list_;
  CardActionManager() {}

 public:
  static CardActionManager& GetCardManager() {
    static CardActionManager card_manager;
    return card_manager;
  }

  // Returns true if the card action is successfully registered.
  bool RegisterCardAction(const string& card_id,
                          const CardAction& card_action) {
    if (card_actions_list_.find(card_id) == card_actions_list_.end()) {
      card_actions_list_[card_id] = card_action;
      return true;
    }
    return false;
  }
};

/*
  How Target Works.

  Target class is passed to the any Action object that requires the target. For
  example, Hit object wants to know what cards it must target. Also when the
  user plays the card,
  sometimes it requires the target to do something (like giving buffs or deals
  damage.)

  In all cases, we need to pass Target object to tell what kind of actions
  it needs to do.


*/
// Target types
// SELECT (the card that the user selected)
//  - User selects one card;
class Target {
  Game* game_;

  // Owner of this Target.
  Card* card_;

  vector<Card*> targets_;
  // bool (const Game* game, Card* my_card, Card* card_need_to_be_checked)
  vector<function<bool(const Game*, Card*, Card*)>> find_targets_;

 public:
  Target(const Game* game) : game_(game), card_(nullptr) {}
  Target(const function<bool(const Game*, Card*, Card*)> find_target)
      : game_(nullptr), card_(nullptr) {
    find_targets_.push_back(find_target);
  }
  Target(const Game* game, const Card* card_) : game_(game), card_(card) {}

  virtual bool FindTargets(const Card* card) {
    if (card_ == nullptr) card_ = card;
    vector<Card*> targets_ = game_.GetField().FindPlayerCard();
    for (int i = 0; i < find_targets_.size(); i++) {
      // Removes the card from the list that fails to pass the check.
      targets_.erase(
          remove_if(targets_.begin(), targets_.end(), [&](Card* to_be_checked) {
            return find_targets_[i](game_, card, to_be_checked);
          }));
    }
  }

  Target& operator+(const Target& target) {
    find_targets_.insert(find_targets_.end(), target.find_targets_.begin(),
                         target.find_targets_.end());
    return *this;
  }

  Target& operator-(const Target& target) {
    for (int i = 0; i < target.find_targets.size(); i++) {
      auto f = target.find_targets_[i];
      find_targets_.push_back(
          [f](const Game* g, Card* me, Card* check) -> bool {
            return !f(g, me, check);
          });
    }
  }

  void SetGame(const Game* game) {
    game_ = game;
  }

  void SetCard(const Card* card) {
    card_ = card;
  }
};

Target ALL_MY_MINIONS([](const Game* game, const Card* owner, const Card* c) {
  if (c->GetZone() == PLAY && c->GetOwnerId() == owner->GetOwnerId()) return true;
  return false;
});

Target ALL_MY_MINIONS([](const Game* game, const Card* owner, const Card* c) {
  if (c->GetZone() == PLAY && c->GetOwnerId() == owner->GetOwnerId()) return true;
  return false;
});



    class Select : Target {
 public:
  Select(const Game* game_) : Target(game, CUSTOM) {}
}

// Examples
// DESTROY (ALL_MINIONS) & DISCARD (MY_CARDS)  (Deathwing)
class Action {
  vector<vector<Action*>> action_list_;
  int current_pointer_;

  void MakeActionList() {
    action_list_ = new vector<vector<Action>>(1);
    current_pointer = 0;
  }

 public:
  Action() : action_list_(nullptr), current_pointer_(0) {}

  // Two actions are done together
  Action& operator&(const Action& action) {
    if (action_list_ == nullptr) MakeActionList();
    action_list_.resize(action_list_.size() + action.action_list_.size() - 1);
    for (int i = 0; i < action.action_list_.size(); i++) {
      action_list_.at(current_pointer_)
          .insert(action_list_.at(current_pointer).end(),  // position
                  action.action_list_.at(i).begin(),       // start
                  action.action_list_.at(i).end()          // end
                  );
      current_pointer_++;
    }
    return *this;
  }

  // Adds an action that happens next.
  Action& operator->(const Action& action) {
    if (action_list == nullptr) MakeActionList();
    action_list_.resize(action_list_->size() + action.action_list_->size());

    for (int i = 0; i < action.action_list_.size(); i++) {
      action_list_.at(current_pointer_) = action.action_list_[i];
      current_pointer_++;
    }

    return *this;
  }

  Action& Hit(const Target& target) { return action; }
  Action& Summon() { return action; }
};

class Conditional {};

class CardAction {
  // ID of the card who owns this action.
  string card_id_;

  Action on_play;
  Action on_hand;
  Action on_deck;

 public:
  CardAction(const string& card_id) : card_id_(card_id) {}

  virtual bool DoAction() = 0;
};

class NEW_CARD : public CardAction {
  NEW_CARD(const string& card_id) : CardAction(card_id) {
    CardActionManager::GetCardManager().RegisterCardAction(card_id, *this);
  }

 public:
  static NEW_CARD& GetCardAction() {
    static NEW_CARD new_card_action(typeid(*this).name());
    return new_card_action;
  }

  NEW_CARD(const NEW_CARD& card) = delete;
  void operator=(const NEW_CARD& card) = delete;
};
