#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;


char map[35][20];

struct point
{
  int x,y;
};

struct player{
  int x,y;
  int toy,tox;
  bool back;
  int score;
  bool search;
  point vec[2];

  //前後左右の確認
  bool inMapX(){
    if (x + vec[0].x >= 0 && x+vec[0].x < 35 && x + vec[1].x >= 0 && x+vec[1].x < 35) {
      return true;
    }else{
      return false;
    }
  }

  bool inMapY(){
    if (y + vec[0].y >= 0 && y+vec[0].y < 20 && y + vec[1].y >= 0 && y+vec[1].y < 20) {
      return true;
    }else{
      return false;
    }
  }

  bool EndSearch(){
    if(map[x+1][y] != '.' && map[x-1][y] != '.' && map[x][y+1] != '.' && map[x][y-1] != '.'){
      return true;
    }else{
      return false;
    }
  }

  void checkVec(bool change){
    if(change || EndSearch()){
      if (x > tox) {
        vec[0].x = -1;
        vec[0].y = 0;
      }else if(x < tox){
        vec[0].x = 1;
        vec[0].y = 0;
      }
      if (y > toy) {
        vec[1].x = 0;
        vec[1].y = -1;
      }else if(y < toy){
        vec[1].x = 0;
        vec[1].y = 1;
      }
      search = false;
    }else if((map[x+vec[0].x][y+vec[0].y] != '.' || !inMapX()) && (map[x+vec[1].x][y+vec[1].y] != '.' || !inMapY())){
      vec[0].x = -vec[0].x;
      vec[1].y = -vec[1].y;
    }
  }

};


void next(player& p){
  if((map[p.x+p.vec[0].x][p.y+p.vec[0].y] != '.' || map[p.x+p.vec[1].x][p.y+p.vec[1].y] != '.') && !p.EndSearch()){
    p.search = true;
  }
  if(p.search){
    if(map[p.x+p.vec[0].x][p.y+p.vec[0].y] == '.' && p.inMapX()){
      p.x += p.vec[0].x;
      p.y += p.vec[0].y;
    }else{
      p.x += p.vec[1].x;
      p.y += p.vec[1].y;
    }
  }else{
    if(p.x != p.tox){
      p.x += p.vec[0].x;
      p.y += p.vec[0].y;
    }else{
      p.x += p.vec[1].x;
      p.y += p.vec[1].y;
    }

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
  me.vec[0].x = 1;
  me.vec[0].y = 0;
  me.vec[1].x = 0;
  me.vec[1].y = -1;
  me.search = false;
  srand((unsigned) time(NULL));
  srand(rand());
  while(1){
    int prescore=0;
    int gameRound;
    int time = 0;
    bool change = false;
    cin >> gameRound; cin.ignore();

    int backInTimeLeft;
    cin >> me.x >> me.y >> backInTimeLeft; cin.ignore();
    //if(backInTimeLeft == 1) change = true;
    if(me.tox == -1 && me.tox == -1){
      me.tox = me.x;
      me.toy = me.y;
    }
    for (int i= 0; i<opponentCount; i++) {
      int opponentX,opponentY,opponentBackInLeft;
      cin >> opponentX >> opponentY >> opponentBackInLeft; cin.ignore();
      //if(opponentBackInLeft == 1) change = true;
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
      change = true;
    }

    me.checkVec(change);

    next(me);

    if (time != 0) {
      cout << "BACK " << time <<endl;
    }else{
      cerr << (!me.inMapX() && !me.inMapY()) << endl;
      cerr << me.search << endl;
      cerr << me.tox << " " << me.toy << endl;
      cerr << me.vec[0].x << " " << me.vec[0].y << endl;
      cerr << me.vec[1].x << " " << me.vec[1].y << endl;
      cerr << me.score << " " << prescore << " " << notfree << endl;
      cout << me.x << " " << me.y << endl;
    }

  }
  return 0;
}
