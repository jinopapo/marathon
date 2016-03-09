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

int nearSoul(State &st,int sx,int sy){
  Point p;
  queue<Search> bfs;
  vector< vector<bool> > closed(st.H, vector<bool>(st.W, false));
  closed[sy][sx] = true;
  bfs.push(Search(sx, sy, 0));
  while (!bfs.empty()) {
    Search sc = bfs.front(); bfs.pop();
    for (int dir = 0; dir < 4; dir++) {
      int nx = sc.x + dx[dir];
      int ny = sc.y + dy[dir];
      if(st.field[sc.y][sc.x].containsSoul) return sc.dist;

      if (!st.field[ny][nx].isEmpty()) continue;
      if (closed[ny][nx]) continue;

      closed[ny][nx] = true;
      bfs.push(Search(nx, ny, sc.dist + 1));
    }
  }
  return 100;
}

void simulateWalk(int id, int dir,State &st) {
  int nx = st.ninjas[id].x + dx[dir];
  int ny = st.ninjas[id].y + dy[dir];
  if (!st.field[ny][nx].isEmpty() && !isMoveStone(st,nx,ny,dir)) return;

  st.ninjas[id].x = nx;
  st.ninjas[id].y = ny;

  if (!st.field[ny][nx].containsSoul) return;

  // 忍力回復
  st.skillPoint += 2;

  // フィールドのフラグをfalseに
  st.field[ny][nx].containsSoul = false;
  st.field[ny][nx].kind = CELL_OBJECT;
  st.field[st.ninjas[id].y][st.ninjas[id].x].kind = CELL_EMPTY;

  // 取得済みのソウルの座標削除
  st.souls.erase( find(st.souls.begin(), st.souls.end(), Point(nx, ny)) );
}

void thinkByNinjaId(int id,int playCount) {
  vector<string> dirs;
  int maxFree=0;
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
    if(maxFree <= nState.free){
      bool contain = false;
      for(nextState ns:nexts){
        if(ns.dir == nState.dir)
          contain = true;
      }
      if(!contain && !nState.nearDogs){
        if(maxFree < nState.free && maxFree < 3){
          maxFree = min(nState.free,3);
          nexts.clear();
        }
        nexts.push_back(nState);
      }
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
    for (auto ns:nss){
      int nx = ns.state.ninjas[id].x;
      int ny = ns.state.ninjas[id].y;
      int dist = nearSoul(ns.state,nx,ny);
      if(minDist >= dist){
        if(minDist > dist){
          soulDists.clear();
          minDist = dist;
        }
        soulDists.push_back(ns);
      }
    }
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
