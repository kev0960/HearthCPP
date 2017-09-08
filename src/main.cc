#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "card.h"
#include "card_info.pb.h"
#include "hscard_db.h"

using namespace std;


int main() {
  const HSCardDB& db = HSCardDB::GetHSCardDB();
  db.Search(SearchHSCard(SearchHSCard::ANY)
                .Greater("cost", 8)
                .Greater("health", 8)
                .Greater("attack", 8));
}
