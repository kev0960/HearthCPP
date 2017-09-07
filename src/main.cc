#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card.h"
#include "card_info.pb.h"
#include "hscard_db.h"

using namespace std;

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

class Action {
  // ID (unique) of the card that created this action.
  string owner_id_;

 public:
  Action(const string& owner_id) : owner_id_(owner_id) {}

  virtual void operator()() = 0;
};

int main() {
  const HSCardDB& db = HSCardDB::GetHSCardDB();
  db.Search(SearchHSCard(SearchHSCard::ANY)
                .Greater("cost", 8)
                .Greater("health", 8)
                .Greater("attack", 8));
}
