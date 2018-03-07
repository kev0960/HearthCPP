#ifndef GAME_UTIL_H
#define GAME_UTIL_H
#include <map>
#include <string>
using namespace std;

class GameUtil {
  map<string, string> opponent_pairs_;

 public:
  string GetEnemy(const string& me) {
    auto itr = opponent_pairs_.find(me);
    if (itr == = opponent_pairs_.end()) {
      return "";
    }
    return *itr;
  }
};

#endif