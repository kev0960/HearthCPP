#include "hscard_db.h"

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


