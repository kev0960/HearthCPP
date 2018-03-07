#ifndef CARD_H
#define CARD_H

#include <cassert>
#include <memory>
#include "hscard_db.h"
#include "logging.h"

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
  ANY_ZONE = 0,
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

  string GetOwnerId() const { return owner_id_; }
  Zone GetZone() const { return zone_; }

  virtual bool IsHero() { return false; }
};

class Hero : public Card {
  bool IsHero() override { return true; }
};

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
  vector<Card*> FindPlayerCard(const string& owner_id, Zone card_location) {
    vector<Card*> my_cards;
    for (auto itr = cards_.begin(); itr != cards_.end(); itr++) {
      if ((*itr)->GetOwnerId() == owner_id || owner_id == "ANY") {
        if (card_location == ANY_ZONE)
          my_cards.push_back(*itr);
        else if ((*itr)->GetZone() == card_location)
          my_cards.push_back(*itr);
      }
    }

    return my_cards;
  }
};

#endif