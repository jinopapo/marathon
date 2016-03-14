#include<iostream>
#include<vector>
#include<stack>
#include<queue>
#include"codevs.h"
#include"state.h"

using namespace std;

//const int INF = 876765346;

int remTime;
vector<Skill> skills;
State myState;
State rivalState;

int dx[] =    {  0,   1,   0,  -1,   0};
int dy[] =    { -1,   0,   1,   0,   0};
string ds[] = {"U", "R", "D", "L",  ""};

void StoneRival(State &st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isStone()) {
    x = rand()%14;
    y = rand()%17;
  }
  st.field[y][x].Empty();
  cout << 2 << " " << y << " " << x << endl;
}

void ThunderMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (!st.field[y][x].isStone()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 3 << " " << y << " " << x << endl;
}

void DummyMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 5 << " " << y << " " << x << endl;
}

void WanwanWarp(State &st,int sid){
  int maskx[] = {-1,0,1,-1,0,1,-1,0,1};
  int masky[] = {1,1,1,0,0,0,-1,-1,-1};
  for(int i=0;i<9;i++){
    int nx = st.ninjas[sid].x + maskx[i];
    int ny = st.ninjas[sid].y + masky[i];
    st.field[ny][nx].containsDog = false;
  }
  cout << 3 << endl;
  cout << 7 << " " << sid << endl;
}

int thinkSkil(State &myState,State rivalState,vector<Skill>skills){
  bool wanwan = false;
  int wanwanCount = 0;
  int warpId=0;
  myState.id = 0;
  if(myState.searchNearDogs() >= 2){
    wanwan = true;
    wanwanCount = myState.searchNearDogs();
  }
  myState.id = 1;
  if(myState.searchNearDogs() > wanwanCount && myState.searchNearDogs() >= 2){
      warpId = 1;
      wanwan = true;
  }
  myState.id = 0;
  if(myState.skillPoint >= skills[7].cost && wanwan){
    WanwanWarp(myState,warpId);
  }/*else if(myState.skillPoint >= skills[7].cost)
    case 3:
      cout << 3 << endl;
      ThunderMe(myState);
      break;
    case 5:
      cout << 3 << endl;
      DummyMe(myState);
      break;
      cout << 2 << endl;
    }
    return id;*/
  else{
    cout << 2 << endl;
  }
  return -1;
}


void searchSoulDist(int soulDist[8][17][14]){
  for(int i=0;i<8;i++){
    Point s = myState.souls[i];
    queue<Search> bfs;
    int nx = s.x;
    int ny = s.y;
    if(!myState.field[ny][nx].isEmpty())continue;
    bfs.push(Search(nx,ny,0));
    soulDist[i][ny][nx] = 0;
    while(!bfs.empty()){
      Search sc = bfs.front();
      bfs.pop();
      for(int dir=0;dir<4;dir++){
        nx = sc.x + dx[dir];
        ny = sc.y + dy[dir];
        if(!myState.field[ny][nx].isEmpty() /*||  myState.field[ny][nx].containsDog*/) continue;
        if(soulDist[i][ny][nx] < sc.dist) continue;
        soulDist[i][ny][nx] = sc.dist+1;
        bfs.push(Search(nx, ny, sc.dist+1));
      }
    }
  }
}


void think(){
  thinkSkil(myState,rivalState,skills);
  queue<State> bfs;
  vector<State> next;
  int soulDist[8][17][14];
  int INF = 876765346;
  for(int i=0;i<8;i++)for(int y=0;y<17;y++)for(int x=0;x<14;x++) soulDist[i][y][x] = INF;
  searchSoulDist(soulDist);
  bfs.push(myState);
  if(!myState.searchNearDogs()){
    myState.id=1;
    bfs.push(myState);
  }
  while(!bfs.empty()){
    State st = bfs.front();
    bfs.pop();
    int nx = st.ninjas[st.id].x;
    int ny = st.ninjas[st.id].y;
    if(st.id == 1 && !st.searchNearDogs() && !st.field[ny][nx].containsDog){
      auto iter = find(next.begin(),next.end(),st);
      int ind = iter - next.begin();
      if(iter != next.end() &&st.skillPoint > next[ind].skillPoint){
        next.erase(iter);
      }
      if(iter == next.end()){
        State ist = st;
        ist.dogSimulate();
        for(int i=0;i<8;i++){
          auto iter = find(ist.getSouls.begin(),ist.getSouls.end(),i);
          if(iter != ist.getSouls.end()) continue;
          int x0 = ist.ninjas[0].x;
          int y0 = ist.ninjas[0].y;
          int x1 = ist.ninjas[1].x;
          int y1 = ist.ninjas[1].y;
          if(soulDist[i][y0][x0] < soulDist[i][y1][x1])
            ist.ninjasSouls[0].push_back(i);
          else
            ist.ninjasSouls[1].push_back(i);
        }
        next.push_back(ist);
      }
    }
    if(st.ninjas[st.id].dir.size() < 2){
      for(int dir=0;dir<4;dir++){
        nx = st.ninjas[st.id].x + dx[dir];
        ny = st.ninjas[st.id].y + dy[dir];
        State ist = st;
        if(ist.field[ny][nx].isWall()) continue;
        if(!ist.isMove(nx,ny,dir)) continue;
        if(ist.field[ny][nx].containsDog) continue;
        ist.simulateWalk(dir);
        int dogs = ist.searchNearDogs();
        ist.ninjas[ist.id].dir.push_back(ds[dir]);
        if(ist.ninjas[ist.id].dir.size() <= 2)
          bfs.push(ist);
        if(ist.id == 0 && !dogs){
          ist.id = 1;
          bfs.push(ist);
        }
      }
    }
  }
  vector<State> tmp;
  for(State st:next){
    bool flag = true;
    for(int i=0;i<2;i++){
      int free = st.searchNearFree(i);
      if(!free) flag = false;
      if(!st.ninjas[i].dir.size()) flag = false;
    }
    if(flag)tmp.push_back(st);
  }
  if(tmp.size())next=tmp;
  tmp.clear();
  int maxSoul = 0;
  for(State st:next){
    if(maxSoul < st.skillPoint){
      maxSoul = st.skillPoint;
      tmp.clear();
    }
    if(maxSoul == st.skillPoint){
      tmp.push_back(st);
    }
  }
  if(tmp.size())next=tmp;
  tmp.clear();
  int minDist = INF*2;
  for(State st:next){
    int dist=0;
    for(int i=0;i<2;i++){
      int d=INF;
      for(int ind:st.ninjasSouls[i]){
        if(d > soulDist[ind][st.ninjas[i].y][st.ninjas[i].x])
          d = soulDist[ind][st.ninjas[i].y][st.ninjas[i].x];
      }
      dist += d;
    }
    if(minDist > dist){
      minDist = dist;
      tmp.clear();
    }
    if(minDist == dist){
      tmp.push_back(st);
    }
  }
  cerr << next.size() << endl;
  if(tmp.size())next=tmp;
  if(next.size()){
    State st = next[rand()%next.size()];
    for(int i=0;i<2;i++){
      for(auto s:st.ninjas[i].dir){
        cout << s;
      }
      cout << endl;
    }
  }else{
    cout << endl;
    cout << endl;
  }
}

bool input() {
  if (!(cin >> remTime)) return false;
  int numOfSkills;
  cin >> numOfSkills;
  skills.clear();
  for (int i = 0; i < numOfSkills; i++) {
    skills.push_back(Skill::input(i));
  }
  myState = State::input(skills.size());
  myState.id = 0;
  rivalState = State::input(skills.size());
  return true;
}


int main(){
  cout << "ji_no_papo" << endl;
  cout.flush();
  while(input()){
    think();
    cout.flush();
  }
  return 0;
}
