#include "hscard_db.h"
#include <iostream>
#define CHECK_FIELD_NAME_AND_SET(_field_name) \
  do {                                        \
    if (field_name == #_field_name) {         \
      card_info->set_##_field_name(value);    \
      return true;                            \
    }                                         \
  } while (0);

const map<string, string> HSCardDB::tag_name_to_field_name = {
    {"HEALTH", "health"},
    {"ATK", "attack"},
    {"COST", "cost"},
    {"CARD_SET", "card_set"},
    {"CLASS", "class_name"},
    {"CARDTYPE", "card_type"},
    {"RARITY", "rarity"},
    {"COLLECTIBLE", "collectible"},
    {"CARDRACE", "card_race"},
    {"CHARGE", "charge"},
    {"INSPIRE", "inspire"},
    {"BATTLECRY", "battlecry"},
    {"DEATHRATTLE", "deathrattle"},
    {"DIVINE_SHIELD", "divine_shield"},
    {"STEALTH", "stealth"},
    {"CHOOSE_ONE", "choose_one"},
    {"DISCOVER", "discover"},
    {"AURA", "aura"},
    {"ELITE", "elite"},
    {"TAUNT", "taunt"},
    {"DURABILITY", "durability"},
    {"POISONOUS", "poison"},
    {"ADAPT", "adapt"},
    {"CANT_BE_TARGETED_BY_SPELLS", "cant_targeted_by_spell"},
    {"CANT_BE_TARGETED_BY_HERO_POWERS", "cant_targeted_by_hero_power"},
    {"AFFECTED_BY_SPELL_POWER", "affected_by_spell_power"},
    {"FREEZE", "freeze"},
    {"SILENCE", "silence"},
    {"WINDFURY", "windfury"},
    {"COMBO", "combo"},
    {"SECRET", "secret"},
    {"OVERLOAD", "overload"},
    {"OVERLOAD_OWED", "overload_owed"},
    {"IMMUNE", "immune"},
    {"CANT_ATTACK", "cant_attack"},
    {"ENRAGED", "enraged"},
    {"QUEST", "quest"},
    {"SPELLPOWER", "spellpower"},
    {"RECEIVES_DOUBLE_SPELLDAMAGE_BONUS", "double_spell_dmg"},
    {"ARMOR", "armor"}};

string SafeLookupMap(const google::protobuf::Map<string, string> &dat,
                     const string &key) {
  if (dat.find(key) != dat.end()) {
    return dat.at(key);
  }
  return "";
}

bool SetI18nFields(tinyxml2::XMLElement *element, const string &field_name,
                   google::protobuf::Map<string, string> *i18n_map) {
  tinyxml2::XMLElement *tag = element->FirstChildElement("Tag");
  bool found = false;
  for (; tag != nullptr; tag = tag->NextSiblingElement()) {
    if (tag->Attribute("name") == nullptr) continue;
    if (string(tag->Attribute("name")) == field_name) {
      tinyxml2::XMLElement *translation = tag->FirstChildElement();
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

bool SetSimpleFields(const string &field_name, int value, CardInfo *card_info) {
  CHECK_FIELD_NAME_AND_SET(health);
  CHECK_FIELD_NAME_AND_SET(attack);
  CHECK_FIELD_NAME_AND_SET(cost);
  CHECK_FIELD_NAME_AND_SET(card_set);
  CHECK_FIELD_NAME_AND_SET(class_name);
  CHECK_FIELD_NAME_AND_SET(card_type);
  CHECK_FIELD_NAME_AND_SET(rarity);
  CHECK_FIELD_NAME_AND_SET(collectible);
  CHECK_FIELD_NAME_AND_SET(hero_power_damage);
  CHECK_FIELD_NAME_AND_SET(card_race);
  CHECK_FIELD_NAME_AND_SET(durability);

  // Boolean values here
  CHECK_FIELD_NAME_AND_SET(charge);
  CHECK_FIELD_NAME_AND_SET(inspire);
  CHECK_FIELD_NAME_AND_SET(battlecry);
  CHECK_FIELD_NAME_AND_SET(deathrattle);
  CHECK_FIELD_NAME_AND_SET(divine_shield);
  CHECK_FIELD_NAME_AND_SET(stealth);
  CHECK_FIELD_NAME_AND_SET(choose_one);
  CHECK_FIELD_NAME_AND_SET(discover);
  CHECK_FIELD_NAME_AND_SET(aura);
  CHECK_FIELD_NAME_AND_SET(elite);
  CHECK_FIELD_NAME_AND_SET(taunt);
  CHECK_FIELD_NAME_AND_SET(poison);
  CHECK_FIELD_NAME_AND_SET(adapt);
  CHECK_FIELD_NAME_AND_SET(cant_targeted_by_spell);
  CHECK_FIELD_NAME_AND_SET(cant_targeted_by_hero_power);
  CHECK_FIELD_NAME_AND_SET(affected_by_spell_power);
  CHECK_FIELD_NAME_AND_SET(freeze);
  CHECK_FIELD_NAME_AND_SET(silence);
  CHECK_FIELD_NAME_AND_SET(windfury);
  CHECK_FIELD_NAME_AND_SET(combo);
  CHECK_FIELD_NAME_AND_SET(secret);
  CHECK_FIELD_NAME_AND_SET(overload);
  CHECK_FIELD_NAME_AND_SET(overload_owed);
  CHECK_FIELD_NAME_AND_SET(immune);
  CHECK_FIELD_NAME_AND_SET(cant_attack);
  CHECK_FIELD_NAME_AND_SET(enraged);
  CHECK_FIELD_NAME_AND_SET(quest);
  CHECK_FIELD_NAME_AND_SET(spellpower);
  CHECK_FIELD_NAME_AND_SET(double_spell_dmg);
  CHECK_FIELD_NAME_AND_SET(armor);

  return false;
}

HSCardDB::HSCardDB(const string &file_name) {
  card_doc_.LoadFile(file_name.c_str());
  if (card_doc_.Error()) {
    cout << "Card Doc is not loaded!" << endl;
    return;
  }

  cout << "Card doc is loaded!" << endl;
  // Build a database of the cards.
  tinyxml2::XMLElement *root = card_doc_.FirstChildElement("CardDefs");
  tinyxml2::XMLElement *element = root->FirstChildElement("Entity");
  while (element != nullptr) {
    CardInfo card_info = ParseCardElement(element);
    card_info_map_[card_info.card_id()] = card_info;
    element = element->NextSiblingElement("Entity");
  }
}

CardInfo HSCardDB::ParseCardElement(tinyxml2::XMLElement *element) {
  CardInfo card_info;
  card_info.set_card_id(element->Attribute("CardID"));
  SetI18nFields(element, "CARDNAME", card_info.mutable_card_name());
  SetI18nFields(element, "FLAVORTEXT", card_info.mutable_flavor_text());
  SetI18nFields(element, "CARDTEXT_INHAND",
                card_info.mutable_card_text_inhand());

  // cout << SafeLookupMap(card_info.card_name(), "koKR") << endl;
  // cout << SafeLookupMap(card_info.flavor_text(), "koKR") << endl;
  // cout << SafeLookupMap(card_info.card_text_inhand(), "koKR") << endl;

  tinyxml2::XMLElement *tag = element->FirstChildElement("Tag");
  for (; tag != nullptr; tag = tag->NextSiblingElement()) {
    const char *tag_name = tag->Attribute("name");
    if (tag_name != nullptr) {
      auto itr = tag_name_to_field_name.find(string(tag_name));
      if (itr != tag_name_to_field_name.end()) {
        SetSimpleFields(itr->second, tag->IntAttribute("value"), &card_info);
      }
    }
  }
  return card_info;
}

vector<CardInfo> HSCardDB::Search(const vector<string> &card_id,
                                  const SearchHSCard &criteria) {
  map<string, CardInfo> found;
  for (auto itr = card_id.begin(); itr != card_id.end(); itr++) {
    if (card_info_map_.find(*itr) != card_info_map_.end()) {
      found[*itr] = card_info_map_[*itr];
    }
  }
  return Search(found, criteria);
}
vector<CardInfo> HSCardDB::Search(const SearchHSCard &criteria) {
  return Search(card_info_map_, criteria);
}

vector<CardInfo> HSCardDB::Search(const map<string, CardInfo> &map,
                                  const SearchHSCard &criteria) {
  vector<CardInfo> found;
  for (auto itr = map.begin(); itr != map.end(); itr++) {
    if (criteria.meet(itr->second)) found.push_back(itr->second);
  }
  for (auto itr = found.begin(); itr != found.end(); itr ++) {
    cout << "Found :: " << SafeLookupMap(itr->card_name(), "koKR") << endl;
  }
  // cout << SafeLookupMap(card_info.card_name(), "koKR") << endl;
  return found;
}

SearchHSCard &SearchHSCard::Exact(string field_name, const set<int> &matches) {
  criterias_.push_back(make_pair(field_name, [&matches](int card_val) -> bool {
    // If card_val is included in matches, returns true.
    return matches.find(card_val) != matches.end();
  }));

  return *this;
}
SearchHSCard &SearchHSCard::Lower(string field_name, int upper_bound) {
  criterias_.push_back(make_pair(
      field_name,
      [upper_bound](int card_val) -> bool { return card_val < upper_bound; }));

  return *this;
}
SearchHSCard &SearchHSCard::Greater(string field_name, int lower_bound) {
  criterias_.push_back(make_pair(
      field_name,
      [lower_bound](int card_val) -> bool { return card_val > lower_bound; }));

  return *this;
}

// a <= x < b
SearchHSCard &SearchHSCard::Between(string field_name, int lower, int upper) {
  criterias_.push_back(
      make_pair(field_name, [lower, upper](int card_val) -> bool {
        return lower <= card_val && card_val < upper;
      }));

  return *this;
}

bool SearchHSCard::meet(const CardInfo &card_info) const {
  const auto *descriptor = card_info.GetDescriptor();
  const auto *reflection = card_info.GetReflection();

  for (auto itr = criterias_.begin(); itr != criterias_.end(); itr++) {
    const auto *field_desc = descriptor->FindFieldByName(itr->first);
    if (field_desc == nullptr) {
      if (cond_ == ALL) return false;
      continue;
    }
    bool result = false;
    if (field_desc->type() == google::protobuf::FieldDescriptor::TYPE_BOOL) {
      bool val = reflection->GetBool(card_info, field_desc);
      result = itr->second(static_cast<int>(val));
    } else if (field_desc->type() ==
               google::protobuf::FieldDescriptor::TYPE_INT32) {
      int val = reflection->GetInt32(card_info, field_desc);
      result = itr->second(val);
    }

    if (result && cond_ == ANY) return true;
    if (!result && cond_ == ALL) return false;
  }

  if (cond_ == ANY) return false;
  if (cond_ == ALL) return true;
  return false;
}
