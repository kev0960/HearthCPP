#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card_info.pb.h"
#include "hscard_db.h"
using namespace std;

class Card {
  string card_id;
  CardInfo card_info;

 public:
  Card(const string &card_id) : card_id(card_id) {}

  // Returns the owner of the card.
  virtual int owner() const = 0;
};


class Field {
  vector<Card *> cards;
};
int main() {
  HSCardDB db ("CardDefs.xml");
  db.Search(SearchHSCard(SearchHSCard::ALL).Exact("cost", set<int>{3}));
}
