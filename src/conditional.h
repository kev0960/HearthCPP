#ifndef CONDITIONAL_H
#define CONDITIONAL_H
#include "card.h"
#include "target.h"

// Conditionals
// Takes the object and the function to check whether the expression is true.
// Examples
//  To(USER_TARGET).Do(If(TARGET(IS_DEMON)).Then(DESTROY()))
//  To(USER_MINION_TARGET).Do(To(NEARBY_PREV_TARGET + PREV_TARGET).Do(FREEZE()))

using CondFuncType = function<bool(Card*)>;
class Conditional {
 protected:
  Target* target_;
  CondFuncType cond_func_;

 public:
  Conditional(CondFuncType cond_func);
  virtual bool Check();
  virtual void SetTarget(Target* target);

  virtual ~Conditional();
};

// Refers to the target selected by "Do"
// Returns true if the all of the target specified by "Do" passes
// the condition. Note that if there are zero targets, then it returns false.
class ConditionalAllTarget : public Conditional {
 public:
  ConditionalAllTarget(function<bool(Card*)> cond);
  bool Check() override;
};

// Refers to the target selected by "Do"
// Returns true if any of the target specified by "Do" passes
// the condition. Note that if there are zero targets, it returns false.
class ConditionalAnyTarget : public Conditional {
 public:
  ConditionalAnyTarget(CondFuncType cond);
  bool Check() override;
};

class NumMyMinions : public Conditional {
  function<bool(int)> chk_num_;

 public:
  NumMyMinions(function<bool(int)> cond);
  bool Check() override;

  // Since the targets is already set as ALL_MY_MINIONS,
  // we dont have to set targets.
  void SetTarget(Target* target) override;
  ~NumMyMinions();
};

class NumEnemyMinions : public Conditional {
  function<bool(int)> chk_num_;

 public:
  NumEnemyMinions(function<bool(int)> cond);
  bool Check() override;
  void SetTarget(Target* target) override;

  ~NumEnemyMinions();
};

#endif