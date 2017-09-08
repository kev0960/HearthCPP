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

// The instantiated card which exists in Game.
class Card {
  // Unique ID of the card which is assigned every time the card
  // is generated.
  string uid_;
  CardInfo card_info_;
  string owner_id_;

  enum Zone {
    PLAY, // Game field
    DECK,
    HAND,
    SECRET,
    GRAVEYARD,
    REMOVED_FROM_GAME,
    SET_ASIDE
  };

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
};

class Hero : public Card {

};

class Field {
  vector<Card*> cards_;

  public:

  vector<Card*> FindPlayerCard (const string& owner_id) {
    vector<Card*> my_cards;
    for (auto itr = cards_.begin(); itr != cards_.end(); itr ++) {
      if ((*itr)->GetOwnerId() == owner_id) my_cards.push_back(*itr);
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

  const Game& game_;
  TargetTypes target_types_;

  // User-defined custom function.
  function<bool(const Game&, vector<Card>&)> custom_function_;

  // Found targets.
  vector<Card> targets_;

  // Initialized when this object is actually used.
  string owner_id_;

 public:
  Target(const Game& game, TargetTypes target_types) : game_(game),
    target_types_(target_types) {}

  Target(const Game& game, TargetTypes target_types,
         const function<bool(const Game&, vector<Card>&)>& custom_function) :
  game_(game), target_types_(target_types), custom_function_(custom_function) { }

  bool FindTargets(const string& owner_id) {
    owner_id_ = owner_id;
    switch (target_types_) {
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
        custom_function_(game_, targets_);
        break;
      default:
        return false;
    }
    return true;
  }

  void all_my_minions() {
    if (owner_id_.empty()) {
      return;
    }    
  }

  void all_enemy_minions() {
  }

  void my_hero() {
  }

  void enemy_hero() {
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
