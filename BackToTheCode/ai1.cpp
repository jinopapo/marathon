#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
  int opponentCount;
  cin >> opponentCount; std::cin.ignore();
  int toy = -1;
  int tox = -1;
  bool back = false;
  int notfree = 0;
  while(1){
    int gameRound;
    int time = 0;
    cin >> gameRound; cin.ignore();
    int x,y;
    int nextx,nexty;
    int vecx,vecy;
    char map[35][20];
    int backInTimeLeft;
    cin >> x >> y >> backInTimeLeft; cin.ignore();
    if(tox == -1 && tox == -1){
      tox = x;
      toy = y;
    }
    nextx = x;
    nexty = y;
    if(map[x][y] != '.'){
      notfree++;
    }else{
      notfree = 0;
    }
    for (int i= 0; i<opponentCount; i++) {
      int opponentX,opponentY,opponentBackInLeft;
      cin >> opponentX >> opponentY >> opponentBackInLeft; cin.ignore();
    }
    for (int i=0; i<20; i++) {
      string line;
      cin >> line; cin.ignore();
      for (int j=0; j<35; j++) {
        map[j][i] = line[j];
      }
    }

    if((tox == x && toy == y) || notfree == 10){
      if(rand() % 3 == 1 && !back && gameRound > 30 && map[tox][toy] != '.' && notfree != 10){
        time = rand() % 16 + 10;
        back = true;
      }
      while(map[tox][toy] != '.'){
        tox = rand() % 35;
        toy = rand() % 20;
      }
    }

    if (x > tox) {
      vecx = -1;
    }else {
      vecx = 1;
    }
    if (y > toy) {
      vecy = -1;
    }else {
      vecy = 1;
    }

    if(map[nextx+vecx][nexty] != '.' || nextx == tox){
      if (map[nextx+vecx][nexty] != '.' && map[nextx-vecx][nexty] == '.' && nextx != 0 && nextx != 34) {
        nextx -= vecx;
      }else if (map[nextx][nexty+vecy] != '.' && map[nextx][nexty-vecy] == '.' && nexty != 0 && nexty != 19) {
        nexty -= vecy;
      }else if (nexty != toy) {
        nexty += vecy;
      }else{
        nextx += vecx;
      }
    }else{
      nextx += vecx;
    }

    if (time != 0) {
      cout << "BACK " << time <<endl;
    }else{
      cout << nextx << " " << nexty << endl;
    }

  }
  return 0;
}
