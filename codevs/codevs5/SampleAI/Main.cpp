#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <stack>
#include "Status.h"
#include "skill.h"

using namespace std;

int remTime;
vector<Skill> skills;
State myState;
State rivalState;

int dx[] =    {  0,   1,   0,  -1,   0};
int dy[] =    { -1,   0,   1,   0,   0};
string ds[] = {"U", "R", "D", "L",  ""};

bool isMoveStone(const State &st,int sx,int sy,int dir){
  int nx = sx+dx[dir];
  int ny = sy+dy[dir];
  if(st.field[sy][sx].isWall()) return false;
  return !st.field[ny][nx].isObj();
}


void SearchNear(nextState &st,int sx,int sy){
  st.nearDogs = false;
  st.free = 0;
  for(int dir=0;dir<5;dir++){
    int nx = sx + dx[dir];
    int ny = sy + dy[dir];
    if(dir != 5 && (st.state.field[ny][nx].isEmpty() || isMoveStone(st.state,nx,ny,dir)))
      st.free++;
    if(st.state.field[ny][nx].containsDog)
      st.nearDogs = true;
  }
}

/**
 * (sx, sy) を起点として、幅優先探索で (最も近い忍犬までの距離, 最も近いアイテムまでの距離) を計算して MinDist で返します。
 * 忍犬やアイテムがフィールド上に存在しない場合は、対応する値を INF として返します。
 * (sx, sy) が歩けない場所である場合、 MinDist(-INF, -INF) として返します。
 */
MinDist getMinDist(const State &st, int sx, int sy) {
  if (!st.field[sy][sx].isEmpty()) {
    return MinDist(-INF, -INF);
  }

  MinDist minDist(INF, INF);
  queue<Search> open;
  vector< vector<bool> > closed(st.H, vector<bool>(st.W, false));
  closed[sy][sx] = true;
  open.push(Search(sx, sy, 0));

  while (!open.empty()) {
    Search sc = open.front(); open.pop();

    if (st.field[sc.y][sc.x].containsDog && minDist.dogDist == INF) {
      minDist.dogDist = sc.dist;
    }
    if (st.field[sc.y][sc.x].containsSoul && minDist.soulDist == INF) {
      minDist.soulDist = sc.dist;
    }

    if (minDist.dogDist != INF && minDist.soulDist != INF) return minDist;

    for (int dir = 0; dir < 4; dir++) {
      int nx = sc.x + dx[dir];
      int ny = sc.y + dy[dir];

      if (!st.field[ny][nx].isEmpty()) continue;
      if (closed[ny][nx]) continue;

      closed[ny][nx] = true;
      open.push(Search(nx, ny, sc.dist + 1));
    }
  }

  return minDist;
}

int farFree(State st,int sx,int sy){
  int meanFree = 0;
  for(int monte = 0;monte <100;monte++){
    int doneMap[17][14] = {{false}};
    int nx = sx;
    int ny = sy;
    int free = 0;
    for(int depth = 0;depth<20;depth++){
      vector<int> next;
      doneMap[ny][nx] = true;
      for(int dir = 0;dir<4;dir++){
        Cell cell = st.field[ny+dy[dir]][nx+dx[dir]];
        if(doneMap[ny+dy[dir]][nx+dx[dir]] || cell.isWall()) continue;
        if(!cell.containsDog && (cell.isEmpty() || isMoveStone(st,nx+dx[dir],ny+dy[dir],dir))){
          next.push_back(dir);
          free++;
        }
      }
      if(next.size()){
        int nextDir = next[rand()%next.size()];
        nx = nx+dx[nextDir];
        ny = ny+dy[nextDir];
      }
    }
    meanFree+=free;
  }
  meanFree /= 100;
  return meanFree;
}

void simulateWalk(int id, int dir,State &st) {
  int nx = st.ninjas[id].x + dx[dir];
  int ny = st.ninjas[id].y + dy[dir];
  if (!st.field[ny][nx].isEmpty() && !isMoveStone(st,nx,ny,dir)) return;

  st.ninjas[id].x = nx;
  st.ninjas[id].y = ny;

  if (st.field[ny][nx].containsSoul){
    st.skillPoint += 2;
    st.field[ny][nx].containsSoul = false;
    st.souls.erase( find(st.souls.begin(), st.souls.end(), Point(nx, ny)) );
  }

  if(st.field[ny][nx].isObject() && isMoveStone(st,nx,ny,dir)){
    st.field[st.ninjas[id].y][st.ninjas[id].x].kind = CELL_EMPTY;
    st.field[st.ninjas[id].y+dy[dir]][st.ninjas[id].x+dx[dir]].kind = CELL_OBJECT;
  }
}

int nearSoul(State &st,int id,int sx,int sy){
  struct bfsState{
    State st;
    int dist;
    bool done[17][14];
  };
  Point p;
  queue<bfsState> bfs;
  bfsState bst;
  for(int iy=0;iy<17;iy++)for(int ix=0;ix<14;ix++) bst.done[iy][ix] = false;
  bst.st = st;
  bst.done[sy][sx] = true;
  bst.dist = 0;
  bfs.push(bst);
  while (!bfs.empty()) {
    for (int dir = 0; dir < 4; dir++) {
      bfsState bs = bfs.front();
      int nx = bs.st.ninjas[id].x + dx[dir];
      int ny = bs.st.ninjas[id].y + dy[dir];
      if(bs.st.field[ny][nx].isWall()) continue;
      if(!bs.st.field[ny][nx].isEmpty() && !isMoveStone(bs.st,nx+dx[dir],ny+dy[dir],dir)) continue;
      simulateWalk(id,dir,bs.st);
      if(bs.st.field[ny][nx].containsSoul){
        st.field[ny][nx].containsSoul = false;
        st.souls.erase( find(st.souls.begin(), st.souls.end(), Point(nx, ny)) );
        return bs.dist;
      }

      if (!bs.st.field[ny][nx].isEmpty() || bs.st.field[ny][nx].containsDog) continue;
      if (bs.done[ny][nx]) continue;

      bs.done[ny][nx] = true;
      bs.dist++;
      if(bs.dist < 10)
        bfs.push(bs);
    }
    bfs.pop();
  }
  return 100;
}

void thinkByNinjaId(int id,int playCount) {
  vector<string> dirs;
  vector<int> freeDir;
  stack<nextState> dfs;
  vector<nextState> nexts;
  nextState nState;
  nState.state = myState;
  nState.free = 0;
  nState.nearDogs = false;
  dfs.push(nState);

  while(!dfs.empty()){
    nState = dfs.top();
    dfs.pop();
    bool contain = false;
    for(nextState ns:nexts){
      if(ns.dir == nState.dir)
        contain = true;
    }
    if(!contain && !nState.nearDogs && nState.dir.size()){
      nexts.push_back(nState);
    }
    if((int)nState.dir.size() != playCount){
      for(int dir=0;dir<4;dir++){
        int nx = nState.state.ninjas[id].x + dx[dir];
        int ny = nState.state.ninjas[id].y + dy[dir];
        if(!nState.state.field[ny][nx].isWall()){
          if(nState.state.field[ny][nx].isEmpty() || isMoveStone(nState.state,nx,ny,dir)){
            nextState ns =  nState;
            simulateWalk(id,dir,ns.state);
            ns.dir.push_back(ds[dir]);
            SearchNear(ns,nx,ny);
            dfs.push(ns);
          }
        }
      }
    }
  }

  if(nexts.size() != 0){
    int maxSoul = 0;
    int minDist = 100;
    vector<nextState> nss;
    vector<nextState> soulDists;
    for(auto ns:nexts){
      if(maxSoul <= ns.state.skillPoint){
        if(maxSoul < ns.state.skillPoint){
          nss.clear();
          maxSoul = ns.state.skillPoint;
        }
        nss.push_back(ns);
      }
    }
    vector<nextState> monte;
    /*for (auto ns:nss){
      int nx = ns.state.ninjas[id].x;
      int ny = ns.state.ninjas[id].y;
      int free = farFree(ns.state,nx,ny);
      if(free >= 20) monte.push_back(ns);
      }*/
    if(!monte.size()) monte = nss;
    for (auto ns:monte){
      int nx = ns.state.ninjas[id].x;
      int ny = ns.state.ninjas[id].y;
      int dist = nearSoul(ns.state,id,nx,ny);
      if(minDist >= dist){
        if(minDist > dist){
          soulDists.clear();
          minDist = dist;
        }
        soulDists.push_back(ns);
      }
    }
    if(!soulDists.size()) soulDists = monte;
    nState = soulDists[rand()%soulDists.size()];
    myState = nState.state;
    for(string s:nState.dir) cout << s;
  }
}

void think() {
  int moveLoop = 2;

  int id = thinkSkil(myState,rivalState,skills);
  if(id == 0){
    moveLoop = 3;
  }
  for (int i = 0; i < (int)myState.ninjas.size(); i++) {
    thinkByNinjaId(myState.ninjas[i].id,moveLoop);
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
  rivalState = State::input(skills.size());

  return true;
}

int main() {
  cout << "ji_no_papo" << endl;
  cout.flush();

  while (input()) {
    think();
    cout.flush();
  }

  return 0;
}
