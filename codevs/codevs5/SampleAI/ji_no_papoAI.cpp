#include<iostream>
#include<vector>
#include<stack>
#include<queue>
#include"codevs.h"
#include"state.h"

using namespace std;

const int INF = 876765346;

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
        if(!myState.field[ny][nx].isEmpty()) continue;
        if(soulDist[i][ny][nx] < sc.dist) continue;
        soulDist[i][ny][nx] = sc.dist+1;
        bfs.push(Search(nx, ny, sc.dist+1));
      }
    }
  }
}


vector<State> bfsNext(State now,int range=INF){
  queue<State> bfs;
  int done[17][14];
  vector<State> next;
  now.id = 0;
  for(int i=0;i<2;i++){
    now.ninjas[i].dir.clear();
  }
  for(int y=0;y<17;y++)for(int x=0;x<14;x++) done[y][x] = -1;
  bfs.push(now);
  if(!now.searchNearDogs()){
    now.id=1;
    bfs.push(now);
  }
  while(!bfs.empty()){
    State st = bfs.front();
    bfs.pop();
    int nx = st.ninjas[st.id].x;
    int ny = st.ninjas[st.id].y;
    if(st.id == 1 && !st.searchNearDogs()){
      auto iter = find(next.begin(),next.end(),st);
      if(iter == next.end() || next[iter-next.begin()].skillPoint < st.skillPoint){
        while(iter != next.end()){
          next.erase(iter);
          iter = find(next.begin(),next.end(),st);
        }
        done[ny][nx] = st.skillPoint;
        State ist = st;
        ist.dogSimulate();
        next.push_back(ist);
        if(range <= (int)next.size()){
          queue<State> q;
          swap(bfs,q);
          continue;
        }
      }
    }
    if(st.ninjas[st.id].dir.size() < 2){
      for(int dir=0;dir<4;dir++){
        nx = st.ninjas[st.id].x + dx[dir];
        ny = st.ninjas[st.id].y + dy[dir];
        State ist = st;
        if(!ist.isMove(nx,ny,dir)) continue;
        if(ist.field[ny][nx].containsDog) continue;
        ist.simulateWalk(dir);
        if(ist.id == 0 && done[ny][nx] > ist.skillPoint) continue;
        ist.ninjas[ist.id].dir.push_back(ds[dir]);
        if(ist.ninjas[ist.id].dir.size() <= 2)
          bfs.push(ist);
        if(ist.id == 0){
          if(done[ny][nx] == ist.skillPoint) continue;
          if(ist.searchNearDogs()) continue;
          ist.id = 1;
          bfs.push(ist);
        }
        done[ny][nx] = ist.skillPoint;
      }
    }
  }
  return next;
}

vector<State> nextAlive(vector<State> states){
  vector<State> tmp;
  for(State st:states){
    bool flag = true;
    for(int i=0;i<2;i++){
      bool dead = true;
      bool done[17][14] = {{false}};
      queue<State> bfs;
      st.id = i;
      bfs.push(st);
      while(!bfs.empty()){
        State next = bfs.front();
        bfs.pop();
        if(!next.searchNearDogs()){
          queue<State> q;
          swap(bfs,q);
          dead = false;
          continue;
        }
        for(int dir=0;dir<4;dir++){
          int nx = st.ninjas[st.id].x + dx[dir];
          int ny = st.ninjas[st.id].y + dy[dir];
          State ist = st;
          if(done[ny][nx]) continue;
          done[ny][nx] = true;
          if(!ist.isMove(nx,ny,dir)) continue;
          if(ist.field[ny][nx].containsDog) continue;
          ist.simulateWalk(dir);
          if(ist.ninjas[ist.id].dir.size() <= 2)
            bfs.push(ist);
        }
      }
      if(dead) flag = false;
      if(st.ninjas[i].dir.size() < 2) flag = false;
    }
    if(flag && st.isClose()) flag = false;
    if(flag)tmp.push_back(st);
  }
  if(tmp.size()) return tmp;
  else return states;
}

vector<State> getSouls(vector<State> states){
  vector<State> tmp;
  int maxSoul = 0;
  for(State st:states){
    if(maxSoul < st.skillPoint){
      maxSoul = st.skillPoint;
      tmp.clear();
    }
    if(maxSoul == st.skillPoint){
      tmp.push_back(st);
    }
  }
  if(tmp.size()) return tmp;
  else return states;
}

vector<State> nextPattern(vector<State> states){
  vector<State> tmp;
  if(states.size() < 10){
    for(State st:states){
      vector<State> v = bfsNext(st,10);
    if(v.size() >= 10) tmp.push_back(st);
    }
  }
  if(tmp.size()) return tmp;
  else return states;
}

vector<State> nearestSouls(vector<State> states){
  int INF = 876765346;
  vector<State> tmp;
  int minDist = INF*2;
  int soulDist[8][17][14];
  for(int i=0;i<8;i++)for(int y=0;y<17;y++)for(int x=0;x<14;x++) soulDist[i][y][x] = INF;
  searchSoulDist(soulDist);
  for(State st:states){
    int dist=0;
    for(int i=0;i<8;i++){
      auto iter = find(st.getSouls.begin(),st.getSouls.end(),i);
      if(iter != st.getSouls.end()) continue;
      int x0 = st.ninjas[0].x;
      int y0 = st.ninjas[0].y;
      int x1 = st.ninjas[1].x;
      int y1 = st.ninjas[1].y;
      if(soulDist[i][y0][x0] < soulDist[i][y1][x1])
        st.ninjasSouls[0].push_back(i);
      else
        st.ninjasSouls[1].push_back(i);
    }
    for(int i=0;i<2;i++){
      int d=INF;
      for(int ind:st.ninjasSouls[i]){
        int nx = st.ninjas[i].x;
        int ny = st.ninjas[i].y;
        int sx = myState.ninjas[i].x;
        int sy = myState.ninjas[i].y;
        if(d > soulDist[ind][ny][nx] && soulDist[ind][ny][nx] <= soulDist[ind][sy][sx])
          d = soulDist[ind][ny][nx];
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
  if(tmp.size()) return tmp;
  else return states;
}

void think(){
  thinkSkil(myState,rivalState,skills);
  //cout << 2 << endl;
  vector<State> next;
  next = bfsNext(myState);
  next = nextAlive(next);
  next = nextPattern(next);
  next = getSouls(next);
  next = nearestSouls(next);
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
