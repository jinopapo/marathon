#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>

using namespace std;

struct point
{
  int x,y;

  point inv(){
    point p;
    p.x = -x;
    p.y = -y;
    return p;
  }
};

char map[35][20];
point opponentPoints[3];
int opponentScore[3];
int target;


struct player{
  int x,y;
  int toy,tox;
  bool back;
  int score;
  bool search;
  bool attack;
  point vec[2];

  //前後左右の確認
    bool inMap(point v){
    if(0 <= x+v.x && 35 > x+v.x && 0 <= y+v.y && 20 > y+v.y){
      return true;
    }else{
      return false;
    }
  }

  bool EndSearch(){
    if((map[x+1][y] != '.' || x+1 >= 35)&& (map[x-1][y] != '.' || x-1 < 0) && (map[x][y+1] != '.' || y+1 >= 20) && (map[x][y-1] != '.' || y-1 < 0)){
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
    }else if((map[x+vec[0].x][y+vec[0].y] != '.' || !inMap(vec[0])) && (map[x+vec[1].x][y+vec[1].y] != '.' || !inMap(vec[1]))){
      vec[0].x = -vec[0].x;
      vec[1].y = -vec[1].y;
    }
  }

};

bool area(player p){
  bool end[35][20] ={{false}};
  int dx[4] = {1,0,-1,0};
  int dy[4] = {0,1,0,-1};
  stack<point> dfs;
  point now;
  int ans = 0;
  now.x = p.x;
  now.y = p.y;
  dfs.push(now);
  while(!dfs.empty()){
    now = dfs.top();
    dfs.pop();
    for (int i=0; i < 4; i++) {
      if(map[now.x+dx[i]][now.y+dy[i]] == '.' && !end[now.x+dx[i]][now.y+dy[i]]){
        point a;
        a.x = now.x+dx[i];
        a.y = now.y+dy[i];
        dfs.push(a);
        end[a.x][a.y] = true;
        ans++;
      }
    }
    if(ans >= 100)break;
  }
  cerr << ans << endl;
  if(ans >= 100) return false;
  return true;
}

void next(player& p,int gameRound,int notfree){
  if(/*(map[p.x+p.vec[0].x][p.y+p.vec[0].y] != '.' || map[p.x+p.vec[1].x][p.y+p.vec[1].y] != '.') &&*/ !p.EndSearch() && area(p)){
    p.search = true;
  }
  if(p.search){
    if(map[p.x+p.vec[0].x][p.y+p.vec[0].y] == '.' && p.inMap(p.vec[0])){
      p.x += p.vec[0].x;
      p.y += p.vec[0].y;
    }else{
      p.x += p.vec[1].x;
      p.y += p.vec[1].y;
    }
  }else{
    if(map[p.x+p.vec[0].x][p.y+p.vec[0].y] == '.' && p.inMap(p.vec[0]) && p.x != p.tox){
      p.x += p.vec[0].x;
      p.y += p.vec[0].y;
    }else if(map[p.x+p.vec[1].x][p.y+p.vec[1].y] == '.' && p.inMap(p.vec[1]) && p.y != p.toy){
      p.x += p.vec[1].x;
      p.y += p.vec[1].y;
    }else if (notfree >= 5){
      if(map[p.x-p.vec[0].x][p.y-p.vec[0].y] == '.' && p.inMap(p.vec[0].inv())){
        p.x -= p.vec[0].x;
        p.y -= p.vec[0].y;
      }else if(map[p.x-p.vec[1].x][p.y-p.vec[1].y] == '.' && p.inMap(p.vec[1].inv())){
        p.x -= p.vec[1].x;
        p.y -= p.vec[1].y;
      }else{
        p.x = p.tox;
        p.y = p.toy;
      }
    }else{
      if(p.inMap(p.vec[0]) && p.x != p.tox){
        p.x += p.vec[0].x;
        p.y += p.vec[0].y;
      }else if(p.inMap(p.vec[1]) && p.y != p.toy){
        p.x += p.vec[1].x;
        p.y += p.vec[1].y;
      }
    }

  }
}

int main(){
  int opponentCount;
  int preBack[4] = {0};
  cin >> opponentCount; std::cin.ignore();
  int notfree = 0;
  player me;
  me.attack = true;
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
    if(backInTimeLeft != preBack[0]) change = true;
    preBack[0] = backInTimeLeft;
    if(me.tox == -1 && me.tox == -1){
      me.tox = me.x;
      me.toy = me.y;
    }
    for (int i= 0; i<opponentCount; i++) {
      int opponentX,opponentY,opponentBackInLeft;
      cin >> opponentX >> opponentY >> opponentBackInLeft; cin.ignore();
      opponentPoints[i].x = opponentX;
      opponentPoints[i].y = opponentY;
      opponentScore[i] = 0;
      if(opponentBackInLeft != preBack[i+1]) change = true;
      preBack[i+1] = opponentBackInLeft;
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
        }else if(line[j] != '.'){
          opponentScore[line[j] - '0']++;
        }
      }
    }
    if(me.score == prescore){
      notfree++;
    }else{
      notfree = 0;
    }


    if(((me.tox == me.x && me.toy == me.y) || map[me.tox][me.toy] != '.') && !me.search/*|| notfree == 10*/){
      if(rand() % 3 == 1 && !me.back && gameRound > 30 && map[me.tox][me.toy] != '0' && notfree != 10){
        time = rand() % 16 + 10;
        me.back = true;
      }
      int n=0;
      while(map[me.tox][me.toy] != '.' /*|| notfree == 10*/){
        notfree = 0;
        me.tox = rand() % 35;
        me.toy = rand() % 20;
        n++;
        if(n > 10000) break;
      }
      change = true;
    }

    me.checkVec(change);
    next(me,gameRound,notfree);

    if (time != 0) {
      cout << "BACK " << time <<endl;
    }else{
      cerr << me.search << endl;
      cerr << me.EndSearch() << endl;
      cerr << me.tox << " " << me.toy << endl;
      cerr << me.vec[0].x << " " << me.vec[0].y << endl;
      cerr << me.vec[1].x << " " << me.vec[1].y << endl;
      cout << me.x << " " << me.y << endl;
    }

  }
  return 0;
}
