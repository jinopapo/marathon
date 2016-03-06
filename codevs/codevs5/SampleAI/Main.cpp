#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <stack>
#include "Status.h"

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

/*
 * 引数
 * - id: 忍者ID
 * - dir: 忍者を歩かせる方向
 *
 * ID が id である忍者を dir の方向へ移動するシミュレートを行います。
 * この関数で行われるシミュレート内容
 * - 忍者の位置修正 (移動先が岩の場合は、位置修正を行わずにシミュレートを終了します)
 * - 移動先にニンジャソウルが存在する場合、取得処理(忍力回復する, フィールドのソウルフラグをfalseにする, 取得済みのソウルの座標削除)が行われます。
 * (※簡単なシミュレートのため、壁を押すなどの処理は行われません)
 */
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
  //st.souls.erase( find(myState.souls.begin(), myState.souls.end(), Point(nx, ny)) );
}

/*
 * 移動方向の決め方
 * - 忍者は、自分自身から最も近いニンジャソウルへ向かって移動します。
 * - 壁を押さずに移動します。
 * - 忍犬までの最短距離が1以下になるようなマスへは移動しません。
 * - 自分自身のマスから連結であるマスの中にニンジャソウルが存在しない場合は、忍犬までの最短距離が最大になるように移動します。
 * -- 忍犬も存在しない場合は、その場にとどまります。
 */
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

  //このターン全探索
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
      cerr << endl;
    }
  }

  if(nexts.size() != 0){
    int maxSoul = 0;
    vector<nextState> nss;
    for(auto ns:nexts){
      if(maxSoul <= ns.state.skillPoint){
        nss.push_back(ns);
        maxSoul = ns.state.skillPoint;
      }
    }
    nState = nss[rand()%nss.size()];
    myState = nState.state;
    for(string s:nState.dir) cout << s;
  }
}

/*
 * このAIについて
 * - 各忍者について、 thinkByNinja(id) を2回行います。
 * - thinkByNinja(id) は、一人の忍者の一歩の行動を決める関数です(詳しくは関数のコメントを参照)。
 *
 * - 忍術
 * -- 「超高速」のみを使用します。
 * -- 「超高速」を使えるだけの忍力を所持している場合に自動的に使用して、thinkByNinja(id) を1回多く呼び出します。
 */
void think() {
  int moveLoop = 2;

  if (myState.skillPoint >= skills[0].cost) {
    cout << 3 << endl;
    cout << skills[0].id << endl;
    moveLoop = 3;
  } else {
    cout << 2 << endl;
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
