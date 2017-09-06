#include "hscard_db.h"
#include <memory>

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

class CardActionManager {
  map<string, unique_ptr<CardAction>> card_actions_list_;
  CardActionManager() { }

  public:
    static CardActionManager& GetCardManager() {
      static CardActionManager card_manager;
      return card_manager;
    }

    // Returns true if the card action is successfully registered.
    bool RegisterCardAction(const string& card_id,
        unique_ptr<CardAction>&& card_action) {
      if (card_actions_list_.find(card_id) == card_actions_list_.end()) {
        card_actions_list_[card_id] = std::move(card_action);
        return true;
      }
      return false;
    }
};

class Card {
  // Unique ID of the card which is assigned every time the card
  // is generated.
  string uid;
  CardInfo card_info;

 public:
  // Builds a Card from the card ID.
  Card(const string& card_id) {
    UniqueId& unique_gen = UniqueId::GetUniqueIdGen();
    uid = unique_gen.GetUniqueId();

    HSCardDB& card_db = HSCardDB::GetHSCardDB();
    card_info = card_db.Search(card_id);
  }
};

class CardActionFactory {
  void create_action () {

  }
}
class CardAction {
  // ID of the card who owns this action.
  string card_id_; 

  public:
  CardAction(const string& card_id) : card_id_(card_id) {
    // And registers this card action to the action manager.
    CardActionManager::GetCardManager().RegisterCardAction(
        card_id, *this);
  }

  virtual CardAction* clone() const = 0;
  void operator= (const CardAction& card_action) {
    
  }
};

class NEWCARD : public CardAction {
};
