#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card_info.pb.h"
#include "tinyxml2.h"
#define CHECK_FILED_NAME_AND_SET(_field_name) do { \
  if (field_name == #_field_name) { \
    card_info->set_##_field_name (value); \
    return true; \
  } } while (0);

using namespace std;
class Card {
  string card_id;
  CardInfo card_info;

 public:
  Card(const string& card_id) : card_id(card_id) {}

  // Returns the owner of the card.
  virtual int owner() const = 0;
};

// Builds a Hearthstone Card database from the XML file provided from
// https://raw.githubusercontent.com/HearthSim/hsdata/master/CardDefs.xml
class HSCardDB {
  tinyxml2::XMLDocument card_doc_;
  map<string, CardInfo> card_info_map_;

 public:
  HSCardDB(const string& file_name) {
    card_doc_.LoadFile(file_name.c_str());
    if (card_doc_.Error()) {
      cout << "Card Doc is not loaded!" << endl;
      return;
    }

    cout << "Card doc is loaded!" << endl;
    // Build a database of the cards.
    tinyxml2::XMLElement* root = card_doc_.FirstChildElement("CardDefs");
    tinyxml2::XMLElement* element = root->FirstChildElement("Entity");
    while (element != nullptr) {
      cout << element->Attribute("CardID") << endl;
      CardInfo card_info = ParseCardElement(element);
      card_info_map_[card_info.card_id()] = card_info;
      element = element->NextSiblingElement("Entity");
    }
  }
  CardInfo ParseCardElement(tinyxml2::XMLElement* element) {
    CardInfo card_info;
    card_info.set_card_id(element->Attribute("CardID"));
    SetI18nFields(element, "CARDNAME", card_info.mutable_card_name());
    SetI18nFields(element, "FLAVORTEXT", card_info.mutable_flavor_text());
    SetI18nFields(element, "CARDTEXT_INHAND",
                  card_info.mutable_card_text_inhand());

    cout << SafeLookupMap(card_info.card_name(), "koKR") << endl;
    cout << SafeLookupMap(card_info.flavor_text(), "koKR") << endl;
    cout << SafeLookupMap(card_info.card_text_inhand(), "koKR") << endl;

    return card_info;
  }
  string SafeLookupMap (const google::protobuf::Map<string, string>& dat, const string& key) {
    if (dat.find(key) != dat.end()) {
      return dat.at(key);
    }
    return "";
  }
  bool SetI18nFields(tinyxml2::XMLElement* element, const string& field_name,
                     google::protobuf::Map<string, string>* i18n_map) {
    tinyxml2::XMLElement* tag = element->FirstChildElement("Tag");
    bool found = false;
    for (; tag != nullptr; tag = tag->NextSiblingElement()) {
      if (tag->Attribute("name") == nullptr) continue;
      if (string(tag->Attribute("name")) == field_name) {
        tinyxml2::XMLElement* translation = tag->FirstChildElement();
        while (translation != nullptr) {
          string i18n_code = translation->Name();
          (*i18n_map)[i18n_code] = string(translation->GetText());
          translation = translation->NextSiblingElement();
        }
        found = true;
      }
      if (found) break;
    }
    return true;
  }
  bool SetSimpleFields (const string& field_name, int value, CardInfo* card_info) {
    CHECK_FILED_NAME_AND_SET(health);
    CHECK_FILED_NAME_AND_SET(attack);
    CHECK_FILED_NAME_AND_SET(cost);
    CHECK_FILED_NAME_AND_SET(card_set);
    CHECK_FILED_NAME_AND_SET(class_name);
    CHECK_FILED_NAME_AND_SET(card_type);
    CHECK_FILED_NAME_AND_SET(rarity);
    CHECK_FILED_NAME_AND_SET(collectible);
    CHECK_FILED_NAME_AND_SET(hero_power_damage);
    CHECK_FILED_NAME_AND_SET(card_race);

    // Boolean values here
    CHECK_FILED_NAME_AND_SET(charge);
    CHECK_FILED_NAME_AND_SET(inspire);
    CHECK_FILED_NAME_AND_SET(battlecry);
    CHECK_FILED_NAME_AND_SET(deathrattle);
    CHECK_FILED_NAME_AND_SET(divine_shield);
    CHECK_FILED_NAME_AND_SET(stealth);
    CHECK_FILED_NAME_AND_SET(choose_one);
    CHECK_FILED_NAME_AND_SET(discover);
    CHECK_FILED_NAME_AND_SET(aura);
    CHECK_FILED_NAME_AND_SET(elite);
    CHECK_FILED_NAME_AND_SET(taunt);

    return false;
  }
};
class Field {
  vector<Card*> cards;
};
int main() { HSCardDB card_db("CardDefs.xml"); }
