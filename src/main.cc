#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card_info.pb.h"
#include "hscard_db.h"
#include "card.h"

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

class Field {
  vector<Card*> cards;
};

class Action {
  // ID (unique) of the card that created this action.
  string owner_id_;

  public:
  Action(const string& owner_id) : owner_id_(owner_id) {}

  virtual void operator()() = 0;
};

class Game {
  map<Event::EventType, vector<Action>> listeners_;

 public:
  bool AddEventListener(Event::EventType event_type, const Action& action) {
    listeners_[event_type].push_back(action);
    return true;
  }
};
int main() {
  const HSCardDB& db = HSCardDB::GetHSCardDB();
  db.Search(SearchHSCard(SearchHSCard::ANY)
                .Greater("cost", 8)
                .Greater("health", 8)
                .Greater("attack", 8));
}
