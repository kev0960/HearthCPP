#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card_info.pb.h"
#include "tinyxml2.h"
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
      element = element->NextSiblingElement("Entity");
    }
  }
  bool parseCardElement(tinyxml2::XMLElement* element) {
    CardInfo card_info;
    card_info.set_card_id(element->Attribute("CardID"));
  }
  map<string, string> setI18nFields(tinyxml2::XMLElement* element,
                                    const string& field_name) {
    map<string, string> i18n_to_text;
    tinyxml::XMLElement* tag = element->FirstChildElement("Tag");
    for (; tag != nullptr; tag = tag->NextSiblingElement()) {
      if (String(tag->Attribute("name")) == field_name) {
        tinyxml::XMLElement* translation = tag->FirstChild();
        while (translation != nullptr) {
          i18n_to_text[translation->Name()] = translation->GetText();
        }
      }
    }
    return i18n_to_text;
  }
};
class Field {
  vector<Card*> cards;
};
int main() { HSCardDB card_db("CardDefs.xml"); }
