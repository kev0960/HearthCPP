#include <memory>
#include "hscard_db.h"

class CardAction;
class UniqueId {
  int current_;
  UniqueId() : current_(0) {}

 public:
  static UniqueId& GetUniqueIdGen() {
    static UniqueId unqiue_id;
    return unqiue_id;
  }

  string GetUniqueId() {
    current_++;
    return to_string(current_);
  }

  UniqueId(const UniqueId& u) = delete;
  void operator=(const UniqueId& u) = delete;
};

// The instantiated card which exists in Game.
class Card {
  // Unique ID of the card which is assigned every time the card
  // is generated.
  string uid;
  CardInfo card_info;
  enum Zone {
    PLAY, // Game field
    DECK,
    HAND,
    SECRET,
    GRAVEYARD,
    REMOVED_FROM_GAME,
    SET_ASIDE
  };

  Zone zone;

 public:
  // Builds a Card from the card ID.
  Card(const string& card_id) {
    UniqueId& unique_gen = UniqueId::GetUniqueIdGen();
    uid = unique_gen.GetUniqueId();

    HSCardDB& card_db = HSCardDB::GetHSCardDB();
    card_info = card_db.Search(card_id);
  }
};

class Field {
  vector<Card*> cards;
};

class Game {
  map<Event::EventType, vector<Action>> listeners_;

 public:
  bool AddEventListener(Event::EventType event_type, const Action& action) {
    listeners_[event_type].push_back(action);
    return true;
  }
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


class Target {
  enum TargetTypes {
    ALL,
    ALL_ENEMY_CHARACTERS, // Including the enemy hero
    ALL_MY_CHARACTERS, // Including my hero
    ALL_MINIONS,
    ALL_ENEMY_MINIONS,
    ALL_MY_MINIONS,
    ENEMY_HERO,
    MY_HERO,
    CUSTOM
  };

  TargetTypes types_;
  const Game& game_;

  // User-defined custom function.
  function<bool(const Game&, vector<Card>&)> custom_function_;

  vector<Card> targets_;

 public:
  Target(const Game& game, TargetTypes types) : game_(game),
    types_(types) {}

  Target(const Game& game, TargetTypes types,
         const function<vector<Card>(const Game&)>& custom_function) :
  game_(game), types_(types), custom_function_(custom_function) { }

  bool FindTargets() {
    switch (target_types) {
      case ALL:
        all_my_minions();
        all_enemy_minions();
        my_hero();
        enemy_hero();
        break;
      case ALL_ENEMY_CHARACTERS:
        all_enemy_minions();
        enemy_hero();
        break;
      case ALL_MY_CHARACTERS:
        all_my_minions();
        my_hero();
        break;
      case ALL_ENEMY_MINIONS:
        all_enemy_minions();
        break;
      case ALL_MY_MINIONS:
        all_my_minions();
        break;
      case ENEMY_HERO:
        enemy_hero();
        break;
      case MY_HERO:
        my_hero();
        break;
      case CUSTOM:
        custom_function(Game_, targets_);
        break;
      default:
        return false;
    }
    return true;
  }

  void all_my_minions() {

  }
};


class Action {
 public:
  Action& Hit(const Target& target) {
    return *this;
  }
  Action& Summon() { return *this; }
};

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
