#ifndef HSCARD_DB_H
#define HSCARD_DB_H
#include <google/protobuf/descriptor.h>
#include <cassert>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "card_info.pb.h"
#include "tinyxml2.h"

using namespace std;
class SearchHSCard;

// Builds a Hearthstone Card database from the XML file provided from
// https://raw.githubusercontent.com/HearthSim/hsdata/master/CardDefs.xml
class HSCardDB {
  tinyxml2::XMLDocument card_doc_;
  map<string, CardInfo> card_info_map_;

  const static map<string, string> tag_name_to_field_name;

  vector<CardInfo> Search(const map<string, CardInfo>& map,
                          const SearchHSCard& Criteria) const;

  HSCardDB(const string& file_name);

 public:
  // Returns the HSCardDB Singleton.
  static HSCardDB& GetHSCardDB();
  CardInfo ParseCardElement(tinyxml2::XMLElement* element);

  // We can search for the cards that meets the criteria.
  // E.g "Find the cards with mana cost higher than 4 and
  // has taunt.
  // Search (SearchHSCard(SearchHSCard::ALL).Greater("mana",
  // 4).HasField("taunt");
  vector<CardInfo> Search(const SearchHSCard& criteria) const;
  vector<CardInfo> Search(const vector<string>& card_id,
                          const SearchHSCard& criteria) const;
  CardInfo Search(const string& card_id) const;

  HSCardDB(const HSCardDB& hs) = delete;
  void operator=(const HSCardDB& hs) = delete;
};

class SearchHSCard {
  vector<pair<string, function<bool(int card_val)>>> criterias_;

 public:
  enum Condition { ANY, ALL };

  SearchHSCard(Condition cond = ANY) : cond_(cond) {}
  SearchHSCard& HasField(string field_name);
  SearchHSCard& Exact(string field_name, const set<int>& matches);
  SearchHSCard& Lower(string field_name, int upper_bound);
  SearchHSCard& Greater(string field_name, int lower_bound);

  // a <= x < b
  SearchHSCard& Between(string field_name, int lower, int upper);

  bool meet(const CardInfo& card_info) const;

 private:
  Condition cond_;
};
#endif