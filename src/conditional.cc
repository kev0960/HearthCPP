#include "conditional.h"

Conditional::Conditional(CondFuncType cond_func)
    : target_(nullptr), cond_func_(cond_func) {}

bool Conditional::Check() { return false; }
void Conditional::SetTarget(Target* target) { target_ = target; }
Conditional::~Conditional() {}

/*

  ConditionalAllTarget
  Alias : ALL_TARGET

*/
ConditionalAllTarget::ConditionalAllTarget(function<bool(Card*)> cond)
    : Conditional(cond) {}
bool ConditionalAllTarget::Check() {
  if (target_ == nullptr) {
    LOG("Target is not defined. Must call SetTarget before using!");
  }
  auto targets = target_->GetTargets();
  if (targets.size() == 0) {
    LOG("There are no targets!");
    return false;
  }
  for (Card* target : targets) {
    if (!cond_func_(target)) return false;
  }
  return true;
}

/*

  ConditionalAnyTarget
  Alias : ANY_TARGET

*/
ConditionalAnyTarget::ConditionalAnyTarget(CondFuncType cond)
    : Conditional(cond) {}
bool ConditionalAnyTarget::Check() {
  if (target_ == nullptr) {
    LOG("Target is not defined. Must call SetTarget before using!");
  }
  auto targets = target_->GetTargets();
  if (targets.size() == 0) {
    LOG("There are no targets!");
    return false;
  }
  for (Card* target : targets) {
    if (cond_func_(target)) return true;
  }
  return false;
}

/*

  NumMyMinions
  Alias : NUM_MY_MINIONS

*/

NumMyMinions::NumMyMinions(function<bool(int)> cond)
    : Conditional(nullptr), chk_num_(cond) {
  target_ = new Target(Target::ALL_MY_MINIONS);
}

bool NumMyMinions::Check() {
  auto minions = Target::ALL_MY_MINIONS.GetTargets();
  return chk_num_(minions.size());
}

// Since the targets is already set as ALL_MY_MINIONS,
// we dont have to set targets.
void NumMyMinions::SetTarget(Target* target) {
  // Do nothing.
  return;
}
NumMyMinions::~NumMyMinions() { delete target_; }

/*

  NumEnemyMinions
  Alias : NUM_ENEMY_MINIONS

*/
NumEnemyMinions::NumEnemyMinions(function<bool(int)> cond)
    : Conditional(nullptr), chk_num_(cond) {
  target_ = new Target(Target::ALL_ENEMY_MINIONS);
}

bool NumEnemyMinions::NumEnemyMinions::Check() {
  auto minions = Target::ALL_ENEMY_MINIONS.GetTargets();
  return chk_num_(minions.size());
}

void NumEnemyMinions::SetTarget(Target* target) { return; }
NumEnemyMinions::~NumEnemyMinions() { delete target_; }