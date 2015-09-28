#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct point
{
  int x,y;
};

struct player{
  int x,y;
  int toy,tox;
  int vecx,vecy;
  bool back;
  int score;
  bool search;
};

char map[35][20];


void next(player& p){
  if(map[p.x+p.vecx][p.y] != '.' || p.x == p.tox){
    if (map[p.x][p.y+p.vecy] != '.' && map[p.x-p.vecx][p.y] == '.' && map[p.x-p.vecx][p.y+p.vecy] != '.' && p.x != 0 && p.x != 34) {
      p.x -= p.vecx;
    }/*else if (map[nextx][nexty+vecy] != '.' && map[nextx][nexty-vecy] == '.' && nexty != 0 && nexty != 19) {
       nexty -= vecy;
       }*/else if (p.y != p.toy) {
      p.y += p.vecy;
    }else{
      p.x += p.vecx;
    }
  }else{
    p.x += p.vecx;
  }
}

int main(){
  int opponentCount;
  cin >> opponentCount; std::cin.ignore();
  int notfree = 0;
  player me;
  me.tox = -1;
  me.toy = -1;
  me.back = false;
  me.score = 0;
  while(1){
    int prescore=0;
    int gameRound;
    int time = 0;
    cin >> gameRound; cin.ignore();

    int backInTimeLeft;
    cin >> me.x >> me.y >> backInTimeLeft; cin.ignore();
    if(me.tox == -1 && me.tox == -1){
      me.tox = me.x;
      me.toy = me.y;
    }
    for (int i= 0; i<opponentCount; i++) {
      int opponentX,opponentY,opponentBackInLeft;
      cin >> opponentX >> opponentY >> opponentBackInLeft; cin.ignore();
    }
    prescore = me.score;
    me.score = 0;
    for (int i=0; i<20; i++) {
      string line;
      cin >> line; cin.ignore();
      for (int j=0; j<35; j++) {
        map[j][i] = line[j];
        if(line[j] == '0'){
          me.score++;
        }
      }
    }
    if(me.score == prescore){
      notfree++;
    }else{
      notfree = 0;
    }


    if((me.tox == me.x && me.toy == me.y) || notfree == 10){
      if(rand() % 3 == 1 && !me.back && gameRound > 30 && map[me.tox][me.toy] != '0' && notfree != 10){
        time = rand() % 16 + 10;
        me.back = true;
      }
      while(map[me.tox][me.toy] != '.' || notfree == 10){
        notfree = 0;
        me.tox = rand() % 35;
        me.toy = rand() % 20;
      }
    }

    if (me.x > me.tox) {
      me.vecx = -1;
    }else {
      me.vecx = 1;
    }
    if (me.y > me.toy) {
      me.vecy = -1;
    }else {
      me.vecy = 1;
    }

    next(me);

    if (time != 0) {
      cout << "BACK " << time <<endl;
    }else{
      cerr << me.tox << " " << me.toy << endl;
      cerr << me.score << " " << prescore << " " << notfree << endl;
      cout << me.x << " " << me.y << endl;
    }

  }
  return 0;
}
