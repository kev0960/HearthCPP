#include "target.h"

Target::Target(Game* game)
    : game_(game), card_(nullptr), targets_found_(false) {}
Target::Target(const function<bool(Game*, Card*, Card*)> add_target)
    : game_(nullptr), card_(nullptr), targets_found_(false) {
  add_targets_fn_.push_back(add_target);
}
Target::Target(Game* game, Card* card)
    : game_(game), card_(card), targets_found_(false) {}

Target::Target(const Target& target)
    : card_(target.card_),
      targets_(target.targets_),
      targets_found_(target.targets_found_),
      add_targets_fn_(target.add_targets_fn_),
      exclude_targets_fn_(target.exclude_targets_fn_) {}

bool Target::FindTargets(Card* card) {
  assert(card != nullptr || card_ != nullptr);
  if (card_ == nullptr) card_ = card;
  targets_found_ = true;
  vector<Card*> all_targets_ = game_->GetField().FindPlayerCard();
  set<Card*> found_targets;
  for (size_t i = 0; i < add_targets_fn_.size(); i++) {
    vector<Card*> possible_targets = all_targets_;
    // Removes the card from the list that fails to pass the check.
    possible_targets.erase(
        remove_if(possible_targets.begin(), possible_targets.end(),
                  [&](Card* to_be_checked) {
                    return add_targets_fn_[i](game_, card, to_be_checked);
                  }));

    // Add the accepted list of targets to the set.
    for (auto itr = possible_targets.begin(); itr != possible_targets.end();
         itr++) {
      found_targets.insert(*itr);
    }
  }
  set<Card*> excluding_targets;
  for (size_t i = 0; i < exclude_targets_fn_.size(); i++) {
    vector<Card*> possible_excluding_targets = all_targets_;
    // Removes the card from the list that fails to pass the check.
    possible_excluding_targets.erase(
        remove_if(possible_excluding_targets.begin(),
                  possible_excluding_targets.end(), [&](Card* to_be_checked) {
                    return exclude_targets_fn_[i](game_, card, to_be_checked);
                  }));

    // Add the excluding list of targets to the set.
    for (auto itr = possible_excluding_targets.begin();
         itr != possible_excluding_targets.end(); itr++) {
      excluding_targets.insert(*itr);
    }
    return true;
  }

  // Now exclude the cards in excluding_targets from the
  // possible targets.
  for (auto itr = excluding_targets.begin(); itr != excluding_targets.end();
       itr++) {
    auto found = found_targets.find(*itr);
    if (found != found_targets.end()) {
      found_targets.erase(found);
    }
  }
  copy(found_targets.begin(), found_targets.end(),
       std::back_inserter(targets_));

  return true;
}

vector<Card*> Target::GetTargets() {
  if (!targets_found_) FindTargets(card_);
  return targets_;
}

Target& Target::operator+(const Target& target) {
  add_targets_fn_.insert(add_targets_fn_.end(), target.add_targets_fn_.begin(),
                         target.add_targets_fn_.end());

  exclude_targets_fn_.insert(exclude_targets_fn_.end(),
                             target.exclude_targets_fn_.begin(),
                             target.exclude_targets_fn_.end());
  return *this;
}

Target& Target::operator-(const Target& target) {
  add_targets_fn_.insert(add_targets_fn_.end(),
                         target.exclude_targets_fn_.begin(),
                         target.exclude_targets_fn_.end());

  exclude_targets_fn_.insert(exclude_targets_fn_.end(),
                             target.add_targets_fn_.begin(),
                             target.add_targets_fn_.end());

  return *this;
}

Target Target::ALL_MY_MINIONS([](Game* game, Card* owner, Card* c) {
  if (c->GetZone() == PLAY && c->GetOwnerId() == owner->GetOwnerId())
    return true;
  return false;
});

Target Target::ALL_ENEMY_MINIONS([](Game* game, Card* owner, Card* c) {
  if (c->GetZone() == PLAY &&
      c->GetOwnerId() == game->GetEnemyId(owner->GetOwnerId())) {
    return true;
  }
  return false;
});

Target Target::ALL_MY_CHARACTERS([](Game* game, Card* owner, Card* c) {
  if (c->GetZone() == PLAY && c->GetOwnerId() == owner->GetOwnerId())
    return true;
  if (c->IsHero() && c->GetOwnerId() == owner->GetOwnerId()) return true;
  return false;
});

Target Target::ALL_ENEMY_CHARACTERS([](Game* game, Card* owner, Card* c) {
  string enemy_id = game->GetEnemyId(owner->GetOwnerId());
  if (c->GetZone() == PLAY && c->GetOwnerId() == enemy_id) return true;
  if (c->IsHero() && c->GetOwnerId() == enemy_id) return true;
  return false;
});

UserTarget::UserTarget(
    const function<bool(Game*, Card*, Card*)> target_candidate) : Target(target_candidate) {}