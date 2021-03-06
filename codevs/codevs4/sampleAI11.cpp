//
// codevs 4.0 Sample Program C++
//
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

const int WORKER = 0;
const int KNIGHT = 1;
const int FIGHTER = 2;
const int ASSASSIN = 3;
const int CASTLE = 4;
const int WORKER_FACTORY = 5;
const int WARRIOR_FACTORY = 6;

const int VIEW_RANGE[] = {4, 4, 4, 4, 10, 10, 4};
const int BUILD_COST[] = {40, 20, 40, 60, 999999999, 100, 500};

const int dx[] = {1,0,-1,0};
const int dy[] = {0,1,0,-1};
const int cx[] = {0,1,2,3,0,1,2,0,1,0,9,8,9,7,8,9,6,7,8,9};
const int cy[] = {0,0,0,0,1,1,1,2,2,3,6,7,7,8,8,8,9,9,9,9};
const int CELL = 10;



// C++とJavaのサンプルコードで同じ動作をさせるために同じ擬似乱数を使う。
int lcg = 0;
int rand(int n) {
    lcg = 1664525 * lcg + 1013904223;
    int x = lcg % n;
    return x < 0 ? x + n : x;
}

int Math_abs(int x) {
    return x < 0 ? -x : x;
}

int dist(int y1, int x1, int y2, int x2) {
    return Math_abs(y1 - y2) + Math_abs(x1 - x2);
}

struct point{
  int x;
  int y;

  bool operator==(const int i) const{
    return x == i && y == i;
  }
  bool operator!=(const int i) const{
    return x != i || y != i;
  }
  bool operator!=(const point& p) const{
    return x != p.x || y != p.y;
  }
  bool operator==(const point& p) const{
    return x == p.x && y == p.y;
  }

};

struct mUnit{
  int hp;
  int type;
  bool member;
  int id;
};

struct pointVector{
  vector<point> v;
  int size(){
    return v.size();
  }
  void push(point p){
    bool flag=true;
    for(auto val:v){
      if(p == val) flag = false;
    }
    if(flag) v.push_back(p);
  }
  void pop(point p){
    vector<point> v1;
    for(auto val:v){
      if(p!=val) v1.push_back(val);
    }
    v = v1;
  }
  bool find(int x,int y){
    point p;
    p.x = x;
    p.y = y;
    bool flag = false;
    for(auto val:v){
      if(p==val) flag = true;
    }
    return flag;
  }
};

class Unit {
public:
  // ユニットの現在のステータス。
  // 与えられる入力により input() で設定される。
  int id;
  int y;
  int x;
  int hp;
  int type;
  int set;
  point cellNum;

  // ユニットに出す命令。
  // 移動か生産のどちらかの行動を行うように think() で設定される（どちらの行動もとらない場合もある）。
  int movetoy; // 非負の値ならそのマスへ向かって移動することを表す。
  int movetox; // 非負の値ならそのマスへ向かって移動することを表す。
  int produce; // 非負の値ならそのタイプのユニットを生産することを表す。

  Unit() {
    point p;
    p.x = -1;
    p.y = -1;
    cellNum = p;
    id = -1;
    movetoy = -1;
    movetox = -1;
    produce = -1;
    set = 0;
  }

  bool isMoving() {
    return movetoy >= 0;
  }
  bool isProducing() {
    return produce >= 0;
  }
  bool isFree() {
    return !isMoving() && !isProducing() && set == 0;
  }
  void free() {
    movetoy = -1;
    movetox = -1;
    produce = -1;
    set = 0;
  }

  // 現在設定されている行動を出力用の文字列に変換する。
  // 行動が設定されてない場合は長さ 0 の文字列を返す。
  string toOrderString() {
    stringstream ss;
    if (isProducing()) {
      ss << id << " " << produce;
      return ss.str();
    }
    if (isMoving()) {
      int dy = Math_abs(y - movetoy);
      int dx = Math_abs(x - movetox);
      if (dy + dx > 0) {
        if (rand(dy + dx) < dy) {
          if (y < movetoy) ss << id << " " << "D";
          if (y > movetoy) ss << id << " " << "U";
        } else {
          if (x < movetox) ss << id << " " << "R";
          if (x > movetox) ss << id << " " << "L";
        }
      }
      return ss.str();
    }
    return "";
  }
};

int remainingTime; // 残り持ち時間
int currentResource; // 現在の資源数
Unit myCastle; // 自分の城
Unit opCastle; // 敵の城（敵の城を見つけていないならid=-1
bool isTopLeft; // 1P側か2P側か
map<int, Unit> myUnits; // ユニットIDをキーとする自分のユニットの一覧。
map<int, Unit> opUnits; // ユニットIDをキーとする（視界内の）敵ユニットの一覧。
int  resource[100][100]; // そのマスに中立資源があるなら0,自分の資源なら1,敵の資源なら-1,それ以外は-100,自分の村があるとき2
bool see[100][100]; // そのマスが一度でも視界に入るとtrue
int workerNum[100][100] = {0}; // そのマスにワーカーが何人いるか。移動中のワーカーは目的地のマスでカウント。
bool cellmap[100/CELL][100/CELL]; //セルが探索されているか
bool workerFactoryMap[100][100]; //村があるかなしか
int cellLength = 100/CELL; //セルの大きさ
int attackCell[100/CELL] = {}; //アタッカーが探索する予定のセル
bool attackCellBool[100/CELL][100/CELL] = {false}; //アタッカーが探索したセル
pointVector resourceFreePoint; //ワーカーのいないセルの座標
pointVector resourceNearMyPoint; //自分の陣地側の資源の位置
pointVector resourceNearEmenyPoint; //敵の陣地側の資源の位置
pointVector resourceEmenyPoint; //敵のワーカーのいる資源の位置
pointVector resourceMyPoint; //味方のワーカーのいる資源の位置
int insCellMap[100/CELL][100/CELL]; //このターンに探索を割り当てられるセル
int currentTurn; //現在のターン
vector<mUnit> unitMap[100][100]; //最低限のユニットの情報のマップ
int resourceWarrior; //100ターンいないに拠点を作るための関数
bool onCasle; //敵が城の上に拠点を作っていたらtrue
int searchAssasin; //資源刈りをするアサシンの数
vector<int> resourceIdList; //資源にいるワーカーのid

void stageStart() {
    myUnits.clear();
    opUnits.clear();
    opCastle.id = -1;
    resourceNearMyPoint.v.clear();
    resourceNearEmenyPoint.v.clear();
    resourceFreePoint.v.clear();
    resourceEmenyPoint.v.clear();
    resourceMyPoint.v.clear();
    onCasle = false;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) resource[y][x] = -100;
    for (int y = 0; y < 100/CELL; y++) attackCell[y] = 0;
    for (int y = 0; y < 100/CELL; y++)for (int x = 0; x < 100/CELL; x++) attackCellBool[y][x] = false;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) see[y][x] = false;
    for (int y = 0; y < 100/CELL; y++) for (int x = 0; x < 100/CELL; x++) cellmap[y][x] = false;
    for (int y = 0; y < 100/CELL; y++) for (int x = 0; x < 100/CELL; x++) insCellMap[y][x] = 0;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) workerFactoryMap[y][x] = false;
    resourceWarrior = 500;
    resourceIdList.clear();
}

int nextInt() {
    int x = -1;
    cin >> x;
    return x;
}



bool input() {
  for(int i=0;i<100;i++)for(int j=0;j<100;j++) unitMap[i][j].clear();
  remainingTime = nextInt();
  int currentStage = nextInt();
  currentTurn = nextInt();
  if (currentTurn == 0) {
    // ターンをまたいで維持される変数はステージが新しくなった時点で初期化を行う。
    stageStart();
    // ステージが始まったことをデバッグ出力。
    // （クライアントで実行すると標準エラー出力は ./log/io/ 配下にログが出力される）
    cerr << "stage:" << currentStage << endl;
  }

  for(int i=0;i<100;i++)for(int j=0;j<100;j++) unitMap[i][j].clear();
  currentResource = nextInt();
  {
    // 自分のユニットを読み込み。
    map<int, Unit> mp;
    int myNum = nextInt();
    for (int i = 0; i < myNum; i++) {
      int id = nextInt();
      // 前ターンから知っているユニットはそのまま使う。
      // （ユニットに設定した命令がターンをまたいでも引き継がれる実装）
      Unit u;
      mUnit mu;
      if (myUnits.count(id) > 0) u = myUnits[id];
      u.id = id;
      u.y = nextInt();
      u.x = nextInt();
      u.hp = nextInt();
      u.type = nextInt();
      mp[id] = u;
      mu.hp = u.hp;
      mu.type = u.type;
      mu.member = true;
      mu.id = u.id;
      unitMap[u.y][u.x].push_back(mu);
      if (u.type == CASTLE) {
        myCastle = u;
        // 城が左上と右下のコーナーどちらに近いかで 1P側・2P側を判断
        isTopLeft = dist(myCastle.y, myCastle.x, 0, 0) <  dist(myCastle.y, myCastle.x, 99, 99);
      }else if(u.type == WORKER_FACTORY) workerFactoryMap[u.y][u.x] = true;

      // 一度でも視界に入ったことがあるマスを更新
      int view = VIEW_RANGE[u.type];
      for (int sy = u.y - view; sy <= u.y + view; sy++)
        for (int sx = u.x - view; sx <= u.x + view; sx++)
          if (0 <= sy && sy < 100 && 0 <= sx && sx < 100 && dist(u.y, u.x, sy, sx) <= view)
            see[sy][sx] = true;
    }
    // 死んだユニットをリストから除くため新しい map に追加して、置き換える。
    map<int, Unit> mp1 = myUnits;
    myUnits = mp;
    if(mp1.size() != 0){
      for(map<int, Unit>::iterator it = mp1.begin(); it != mp1.end(); it++) {
        Unit u = it->second;
        if(myUnits.count(u.id) == 0 && u.type == WORKER && !(u.cellNum == -1) && insCellMap[u.cellNum.y][u.cellNum.x] != 0)insCellMap[u.cellNum.y][u.cellNum.x]--;
      }
    }
  }
  {
    // 敵のユニットを読み込み
    map<int, Unit> mp;
    int opNum = nextInt();
    for (int i = 0; i < opNum; i++) {
      int id = nextInt();
      Unit u;
      mUnit mu;
      if (opUnits.count(id) > 0) u = opUnits[id];
      u.id = id;
      u.y = nextInt();
      u.x = nextInt();
      u.hp = nextInt();
      u.type = nextInt();
      mp[id] = u;
      mu.hp = u.hp;
      mu.type = u.type;
      mu.member = false;
      mu.id = u.id;
      unitMap[u.y][u.x].push_back(mu);
      if (u.type == CASTLE) opCastle = u;
    }
    opUnits = mp;
  }
  {
    // 資源の位置を読み込み
    int resNum = nextInt();
    for (int i = 0; i < resNum; i++) {
      int y = nextInt();
      int x = nextInt();
      point p;
      p.x = x;
      p.y = y;
      if(isTopLeft){
        if(dist(p.y,p.x,0,0) < 100) resourceNearMyPoint.push(p);
        else if(dist(p.y,p.x,99,99) < 100) resourceNearEmenyPoint.push(p);
      }else{
        if(dist(p.y,p.x,0,0) < 100) resourceNearEmenyPoint.push(p);
        else if(dist(p.y,p.x,99,99) < 100) resourceNearMyPoint.push(p);
      }
      if(unitMap[y][x].size() == 0){
        resourceFreePoint.push(p);
        resourceMyPoint.pop(p);
        resourceEmenyPoint.pop(p);
      }else{
        int my=0;
        int enemy=0;
        bool enemyw=true;
        for(auto val:unitMap[y][x]){
          if(val.member) my++;
          else enemy++;
          if(val.type == WORKER_FACTORY && !val.member)enemyw=false;
        }
        if(enemy >= my){
          resourceEmenyPoint.push(p);
          resourceFreePoint.pop(p);
          resourceMyPoint.pop(p);
        }else{
          if(enemyw){
            resourceMyPoint.push(p);
            resourceFreePoint.pop(p);
            resourceEmenyPoint.pop(p);
          }else{
            resourceEmenyPoint.push(p);
            resourceFreePoint.pop(p);
            resourceMyPoint.pop(p);
          }
        }
      }
    }
  }

  string end;
  cin >> end;
  if ("END" == end) return true;
  return false;
}

//資源を探す関数
void searchResorce(Unit& u,vector<point> v){
  int min=100;
  int x = -1;
  int y = -1;
  bool flag = true;
  for(auto val:unitMap[u.y][u.x]){
    if(val.id > u.id) flag = false;
  }
  for(auto i:v){
    if(dist(i.y,i.x,u.y,u.x) <= 10){
      if(dist(i.y,i.x,u.y,u.x) != 0){
        if(min > dist(i.y,i.x,u.y,u.x)){
          if(!workerFactoryMap[i.y][i.x] && workerNum[i.y][i.x] < 5){
            min = dist(i.y,i.x,u.y,u.x);
            x = i.x;
            y = i.y;
          }
        }
      }else{
        if(workerNum[i.y][i.x] <= 5){
          min = dist(i.y,i.x,u.y,u.x);
          x = i.x;
          y = i.y;
        }else if(!flag){
          min = dist(i.y,i.x,u.y,u.x);
          x = i.x;
          y = i.y;
        }
      }
    }
  }
  if(x != -1){
    u.movetoy = y;
    u.movetox = x;
    workerNum[u.movetoy][u.movetox]++;
    workerNum[u.y][u.x]--;
    if(insCellMap[u.cellNum.y][u.cellNum.x] != 0){
      insCellMap[u.cellNum.y][u.cellNum.x]--;
    }
  }
}

//4近傍が空いてなかったら城に近づく
void searchCasleWorker(Unit& u,int& rigthWorker,int& leftWorker){
  int sx = -1;
  int sy = -1;
  int n;
  if(isTopLeft) n=10;
  else n=0;
  for(int j=n;j<n+10;j++){
    if(!cellmap[cy[j]][cx[j]]){
      sy = cy[j];
      sx = cx[j];
    }
  }
  if(rigthWorker < leftWorker){
    u.cellNum.y = u.y/CELL;
    if(u.x/CELL+1 <= sx) u.cellNum.x = u.x/CELL+1;
    else if(u.y/CELL+1 <= sy){
      u.cellNum.x = u.x/CELL;
      u.cellNum.y = u.y/CELL + 1;
    }
    rigthWorker--;
  }else if(rigthWorker > leftWorker){
    u.cellNum.x = u.x/CELL;
    if(u.y/CELL+1 <= sy) u.cellNum.y = u.y/CELL+1;
    else if(u.x/CELL+1 <= sx){
      u.cellNum.y = u.y/CELL;
      u.cellNum.x = u.x/CELL+1;
    }
    leftWorker--;
  }else{
    if(u.x/CELL+1 <= sx) u.cellNum.x = u.x/CELL+1;
    else u.cellNum.x = u.x/CELL;
    if(u.y/CELL+1 <= sy) u.cellNum.y = u.y/CELL+1;
    else u.cellNum.y = u.y/CELL;
  }
  if(insCellMap[u.cellNum.y][u.cellNum.x] == 0)insCellMap[u.cellNum.y][u.cellNum.x]++;
  if(u.cellNum != -1) workerNum[u.y][u.x]--;
}

//探索してないセルを探す
void searchCell(Unit& u){
  int min = 100;
  for(int y=0;y<10;y++){
    for(int x=0;x<10;x++){
      if(u.cellNum == -1 &&!cellmap[y][x] && insCellMap[y][x] == 0){
        if(min > dist(u.y/CELL,u.x/CELL,y,x)){
          u.cellNum.x = x;
          u.cellNum.y = y;
          insCellMap[y][x]++;
        }
      }
    }
  }
  if(u.cellNum != -1) workerNum[u.y][u.x]--;
}


void goToResorce(Unit& u,vector<point> v){
  int min=100;
  int x = -1;
  int y = -1;
  for(auto i:v){
    if(workerNum[i.y][i.x] < 5){
      if(min > dist(i.y,i.x,u.y,u.x)){
        min = dist(i.y,i.x,u.y,u.x);
        x = i.x;
        y = i.y;
      }
    }
  }
  if(x != -1){
    u.movetoy = y;
    u.movetox = x;
    workerNum[u.movetoy][u.movetox]++;
    workerNum[u.y][u.x]--;
  }
}

// ワーカーの行動を考える
void thinkWorker() {
  for(int i=0;i<100;i++)for(int j=0;j<100;j++)workerNum[i][j] = 0;
  int leftWorker=0; //左側のワーカーの数
  int rigthWorker=0; //右側のワーカーの数


  // 現状、マスごとにワーカーが何人いるかを数える。
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER){
      if(u.isFree()) workerNum[u.y][u.x]++;
      else{
        workerNum[u.movetoy][u.movetox]++;
        if(workerNum[u.movetoy][u.movetox] > 5){
          u.free();
          workerNum[u.movetoy][u.movetox]--;
        }
      }
      if(u.x/CELL > u.y/CELL){
        rigthWorker++;
      }
      else{
        leftWorker++;
      }
    }
  }
  // 命令を設定
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER) {
      // 目的地についているなら命令を再設定
      if (u.y == u.movetoy && u.x == u.movetox) {
        //資源のついたワーカーの未来の収入を数える
        if(currentTurn <= 100 && resourceMyPoint.find(u.x,u.y) && workerNum[u.y][u.x] <= 5){
          bool flag=true;
          for(auto val:resourceIdList){
            if(val == u.id) flag=false;
          }
          if(flag){
            resourceWarrior += 100 - currentTurn;
            resourceIdList.push_back(u.id);
          }
        }
        u.free();
      }

      //最低限の防御用のワーカーの動き
      if(u.y == myCastle.y && u.x == myCastle.x && currentTurn >= 100){
        u.movetox = u.x;
        u.movetoy = u.y;
      }

      //拠点を一箇所に作るために拠点をつくったワーカーは拠点にとどまる
      for(auto val:unitMap[u.y][u.x]){
        if(val.member && val.type == WARRIOR_FACTORY){
          u.movetox = u.x;
          u.movetoy = u.y;
        }
      }

      if (u.isFree()) {
        //視野に資源があれば一番近い資源に向かい、資源が全部見つかっていたら一番近い資源に向かう
        searchResorce(u,resourceMyPoint.v);
        if(u.movetoy == -1){
          searchResorce(u,resourceFreePoint.v);
        }

        if(u.isFree()){
          //セルを割り当てる
          if(u.cellNum == -1 ){
            //今のセルが探索済みでなかったら今のセルを探索する
            if(!cellmap[u.y/CELL][u.x/CELL] && insCellMap[u.y/CELL][u.x/CELL] == 0){
              u.cellNum.y = u.y/CELL;
              u.cellNum.x = u.x/CELL;
              insCellMap[u.y/CELL][u.x/CELL]++;
            }
            //y方向に動いてるならよりy方向に動くようにする
            for(int i=0;i<4;i++){
              if(!(u.cellNum == -1))continue;
              if(u.y/CELL < u.x/CELL){
                if(u.y/CELL+dy[i] < 0 || u.y/CELL+dy[i] >= CELL || u.x/CELL+dx[i] < 0 || u.x/CELL+dx[i] >= CELL)continue;
                if(!cellmap[u.y/CELL+dy[i]][u.x/CELL+dx[i]] && insCellMap[u.y/CELL+dy[i]][u.x/CELL+dx[i]] == 0){
                  u.cellNum.y = u.y/CELL+dy[i];
                  u.cellNum.x = u.x/CELL+dx[i];
                  insCellMap[u.cellNum.y][u.cellNum.x]++;
                }
              }else{
                if(u.y/CELL+dx[i] < 0 || u.y/CELL+dx[i] >= CELL || u.x/CELL+dy[i] < 0 || u.x/CELL+dy[i] >= CELL)continue;
                if(!cellmap[u.y/CELL+dx[i]][u.x/CELL+dy[i]] && insCellMap[u.y/CELL+dx[i]][u.x/CELL+dy[i]] == 0){
                  u.cellNum.y = u.y/CELL+dx[i];
                  u.cellNum.x = u.x/CELL+dy[i];
                  insCellMap[u.cellNum.y][u.cellNum.x]++;
                }
              }
            }
            if(u.cellNum != -1){
              workerNum[u.y][u.x]--;
            }
            //４近傍に開いてるセルがなく、城が見つかってなかったら城に近づく
            if(u.cellNum == -1 && opCastle.id == -1){
              searchCasleWorker(u,rigthWorker,leftWorker);
            }
            //城が見つかっていたら探索していないセルに向かう
            if(u.cellNum == -1){
              searchCell(u);
            }
            //城が見つかっていたら資源に向かう
            if(u.cellNum == -1){
              goToResorce(u,resourceFreePoint.v);
              if(u.movetoy == -1){
                goToResorce(u,resourceMyPoint.v);
              }
              if(u.movetoy == -1){
                goToResorce(u,resourceEmenyPoint.v);
              }
            }
          }
          //セルに到達したかどうか
          if(u.cellNum.x*CELL+CELL/2 == u.x && u.cellNum.y*CELL+CELL/2 == u.y){
            cellmap[u.cellNum.y][u.cellNum.x] = true;
            if(insCellMap[u.cellNum.y][u.cellNum.x] != 0)insCellMap[u.cellNum.y][u.cellNum.x]--;
            u.cellNum.x = -1;
            u.cellNum.y = -1;
          }else{
            //セルを探索する
            if(Math_abs(u.x - (u.cellNum.x*CELL+CELL/2)) > Math_abs(u.y - (u.cellNum.y*CELL+CELL/2))){
              if(u.x > u.cellNum.x*CELL+CELL/2) u.movetox = u.x-1;
              else u.movetox = u.x+1;
              u.movetoy = u.y;
            }else{
              if(u.y > u.cellNum.y*CELL+CELL/2) u.movetoy = u.y-1;
              else u.movetoy = u.y+1;
              u.movetox = u.x;
            }
          }
        }
      }
    }
  }
}

//城を探す関数
void searchCasleWarrior(Unit& u){
  int sx = -1;
  int sy = -1;
  int n;
  if(isTopLeft) n = 10;
  else n = 0;
  if(u.cellNum == -1){
    int ccelcount = attackCell[0];
    for(int j=n;j<n+10;j++){
      if(attackCell[j-n] < ccelcount && !attackCellBool[cy[j]][cx[j]] && sx == -1){
        sy = cy[j];
        sx = cx[j];
        u.cellNum.x = sx;
        u.cellNum.y = sy;
        attackCell[j-n]++;
      }
    }
    if(sx == -1 && !attackCellBool[cy[0]][cx[0]]){
      u.cellNum.x = cx[n];
      u.cellNum.y = cy[n];
      attackCell[0]++;
    }
  }
}

void goToCasleRoute(vector<point> v,Unit& u,int& ty,int& tx){
  int min = 100;
  for(auto val:v){
    if(opCastle.x > u.x && opCastle.y > u.y){
      if(val.x > u.x && val.x <= opCastle.x && val.y > u.y && val.y <= opCastle.y
         && min > dist(u.y,u.x,val.y,val.x)){
        ty = val.y;
        tx = val.x;
        min = dist(u.y,u.x,val.y,val.x);
      }
    }else if(opCastle.x > u.x && opCastle.y < u.y){
      if(val.x > u.x && val.x <= opCastle.x && val.y < u.y && val.y >= opCastle.y
         && min > dist(u.y,u.x,val.y,val.x)){
        ty = val.y;
        tx = val.x;
        min = dist(u.y,u.x,val.y,val.x);
      }
    }
  }
}

// 戦闘ユニットの行動を考える
void thinkWarrior() {
  int unitCountMap[100][100] = {};
  int assassinMap[100][100] = {};
  vector<point> pv; //拠点の座標
  searchAssasin = 0;
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == KNIGHT || u.type == FIGHTER || (u.type == ASSASSIN && u.set == 2)) {
      unitCountMap[u.y][u.x]++;
    }
    if(u.type == ASSASSIN && u.set == 1){
      if(u.movetox != -1) assassinMap[u.y][u.x]++;
      searchAssasin++;
    }
    if(u.type == WARRIOR_FACTORY){
      point p;
      p.x = u.x;
      p.y = u.y;
      pv.push_back(p);
    }
  }

  // 命令を設定
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;

    if (u.type == KNIGHT || u.type == FIGHTER || (u.type == ASSASSIN && u.set==2)) {
      // 敵の城へ向かう
      int ty = -1, tx = -1;
      if (opCastle.id != -1) {
        goToCasleRoute(resourceEmenyPoint.v,u,ty,tx);
        if(ty == -1) goToCasleRoute(resourceFreePoint.v,u,ty,tx);
        if(ty == -1){
          ty = opCastle.y;
          tx = opCastle.x;
        }
      } else {
        // 敵の城の位置が不明なら敵の城がありそうな場所へ向かう
        if(cellmap[u.cellNum.y][u.cellNum.x]){
          u.cellNum.y = -1;
          u.cellNum.x = -1;
        }
        //城を探す
        searchCasleWarrior(u);
        //セルに到達したかどうか
        if(u.cellNum.x*CELL+CELL/2 == u.x && u.cellNum.y*CELL+CELL/2 == u.y){
          attackCellBool[u.cellNum.y][u.cellNum.x] = true;
          u.cellNum.x = -1;
          u.cellNum.y = -1;
        }else{
          //セルを探索する
          if(Math_abs(u.x - (u.cellNum.x*CELL+CELL/2)) > Math_abs(u.y - (u.cellNum.y*CELL+CELL/2))){
            if(u.x > u.cellNum.x*CELL+CELL/2) u.movetox = u.x-1;
            else u.movetox = u.x+1;
            u.movetoy = u.y;
          }else{
            if(u.y > u.cellNum.y*CELL+CELL/2) u.movetoy = u.y-1;
            else u.movetoy = u.y+1;
            u.movetox = u.x;
          }
        }
        //すべてのセルを探索し終わってたら空いてるマスに向かう
        if(u.cellNum == -1){
          int opCorner = isTopLeft ? 99 : 0;
          for (int y = 0; y < 100; y++) {
            for (int x = 0; x < 100; x++) {
              if (!see[y][x]) {
                if (ty == -1 || dist(opCorner, opCorner, ty, tx) > dist(opCorner, opCorner, y, x)) {
                  ty = y;
                  tx = x;
                }
              }
            }
          }
        }
      }
      //行く場所が決まってたらそこに行く
      if(ty != -1 && tx != -1){
        for(auto val:unitMap[opCastle.y][opCastle.x]){
          if(!val.member && val.type == WARRIOR_FACTORY) onCasle = true;
        }
        if(unitCountMap[u.y][u.x] <= 10 && onCasle){
          u.movetoy = -1;
          u.movetox = -1;
          for(auto i:pv){
            if(u.x == i.x && u.y == i.y){
              u.movetoy = u.y;
              u.movetox = u.x;
            }
          }
          if(u.movetoy == -1){
            if(ty > u.y){
              u.movetoy=u.y+1;
              u.movetox=u.x;
            }else if(ty < u.y){
              u.movetoy=u.y-1;
              u.movetox=u.x;
            }else{
              u.movetoy=u.y;
              if(tx > u.x) u.movetox=u.x+1;
              else if(tx < u.x) u.movetox=u.x-1;
              else u.movetox=u.x;
            }
          }
        }else{
          if(ty > u.y){
            u.movetoy=u.y+1;
            u.movetox=u.x;
          }else if(ty < u.y){
            u.movetoy=u.y-1;
            u.movetox=u.x;
          }else{
            u.movetoy=u.y;
            if(tx > u.x) u.movetox=u.x+1;
            else if(tx < u.x) u.movetox=u.x-1;
            else u.movetox=u.x;
          }
        }
      }
    }

    //資源狩り用のアサシン
    if(u.type == ASSASSIN && u.set == 1){
      if(u.movetox != -1){
        bool flag = true;
        if(resourceMyPoint.find(u.movetox,u.movetoy)) flag = false;

        /*for(auto val:resourceMyPoint.v){
          if(dist(val.y,val.x,u.movetoy,u.movetox) == 0) flag = false;
          }*/
        if(!resourceEmenyPoint.find(u.movetox,u.movetoy)) flag = false;
        if(!flag){
          u.movetox = -1;
          u.movetoy = -1;
        }
        if(resourceEmenyPoint.v.size() == 0 && u.x == u.movetox && u.y == u.movetoy){
          u.movetox = -1;
          u.movetoy = -1;
        }
      }
      if(u.movetox == -1){
        int min=100;
        for(auto val:resourceEmenyPoint.v){
          if(min > dist(u.y,u.x,val.y,val.x) && assassinMap[val.y][val.x] <= 4){
            u.movetox = val.x;
            u.movetoy = val.y;
          }
        }
        assassinMap[u.movetoy][u.movetox]++;
      }
      if(u.movetox == -1){
        int min=100;
        for(int i=0;i<100;i++){
          for(int j=0;j<100;j++){
            if(!see[i][j] && min > dist(i,j,u.y,u.x)){
              min = dist(i,j,u.y,u.x);
              u.movetox = j;
              u.movetoy = i;
            }
          }
        }
      }
      if(u.movetox == -1){
        u.movetox = opCastle.x;
        u.movetoy = opCastle.y;
      }
    }

    //アサシンが生まれてたら資源奪取と攻撃用に振り分ける
    if(u.type == ASSASSIN && u.set == 0){
      if(/*myUnits.size()%4 == 2 &&*/ resourceEmenyPoint.size() != 0 && searchAssasin < 10){
        u.set = 1;
      }else{
        u.set = 2;
      }
    }
  }
}

//敵によって生産するユニットを変える
int SearchEnemyWarrior(){
  int fighter = 0;
  int assasin = 0;
  int knight = 0;
  int ret;
  int maskx[41] = {4,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2,-3,-3,-3,-4};
  int masky[41] = {0,1,0,-1,2,1,0,-1,-2,3,2,1,0,-1,-2,-3,4,3,2,1,0,-1,-2,-3,-4,3,2,1,0,-1,-2,-3,2,1,0,-2,-1,1,0,-1,0};
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    for(int i=0;i<41;i++){
      int x=u.x+maskx[i];
      int y=u.y+masky[i];
      if(x<0) x=0;
      if(x>=100) x=99;
      if(y<0) y=0;
      if(y>=100) y=99;
      for(auto val:unitMap[y][x]){
        if(!val.member){
          switch(val.type){
          case KNIGHT: knight++;break;
          case FIGHTER: fighter++;break;
          case ASSASSIN: assasin++;break;
          }
        }
      }
    }
  }

  if(knight < fighter){
    if(fighter > assasin) ret = ASSASSIN;
    else ret = KNIGHT;
  }else{
    if(knight > assasin) ret = FIGHTER;
    else ret = KNIGHT;
  }

  return ret;
}

//視野に敵がいたらtrue敵が最も多いワーカが拠点を作る
bool SearchEnemy(){
  bool flag = false;
  int maskx[41] = {4,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2,-3,-3,-3,-4};
  int masky[41] = {0,1,0,-1,2,1,0,-1,-2,3,2,1,0,-1,-2,-3,4,3,2,1,0,-1,-2,-3,-4,3,2,1,0,-1,-2,-3,2,1,0,-2,-1,1,0,-1,0};
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if(u.type == WORKER){
      for(int i=0;i<41;i++){
        int x=u.x+maskx[i];
        int y=u.y+masky[i];
        if(x<0) x=0;
        if(x>=100) x=99;
        if(y<0) y=0;
        if(y>=100) y=99;
        for(auto val:unitMap[y][x]){
          if(!val.member && val.type != WORKER && val.type != WORKER_FACTORY){
            flag = true;
          }
        }
      }
    }
  }

  return flag;
}

//生産するユニットを決める関数
void SelectProduce(int count,int type){
  int id=-1;
  int maxdist;
  int distId=-1;
  float max=0;
  int maskx[25] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2};
  int masky[25] = {2,2,2,2,2,1,1,1,1,1,0,0,0,0,0,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2};
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if(u.type == type){
      if(maxdist < dist(myCastle.y,myCastle.x,u.y,u.x)){
        maxdist = dist(myCastle.y,myCastle.x,u.y,u.x);
        distId = u.id;
      }
      float scare=0;
      for(int i=0;i<25;i++){
        int x=u.x/10+maskx[i];
        int y=u.y/10+masky[i];
        if(x<0) x=0;
        if(x>=10) x=9;
        if(y<0) y=0;
        if(y>=10) y=9;
        if(cellmap[y][x]){
          for(auto val:resourceNearMyPoint.v){
            if(y*10 < val.y
               && (y+1)*10 > val.y
               && x*10 < val.x
               && (x+1)*10 > val.x
               && !workerFactoryMap[val.y][val.x]){
              scare+=(5-workerNum[val.y][val.x])/(abs(masky[i])+abs(maskx[i])+1);
            }
          }
          for(auto val:resourceNearEmenyPoint.v){
            if(y*10 < val.y
               && (y+1)*10 > val.y
               && x*10 < val.x
               && (x+1)*10 > val.x
               && !workerFactoryMap[val.y][val.x]){
              scare+=(5-workerNum[val.y][val.x])/(abs(masky[i])+abs(maskx[i])+1);
            }
          }
        }else{
          scare+=1/(abs(masky[i])+abs(maskx[i])+1);
        }
      }
      if(type == WORKER) {
        if(!resourceMyPoint.find(u.x,u.y)) scare=0;
      }
      if(type == WORKER_FACTORY){
        scare+=5-workerNum[u.y][u.x];
      }
      if(max < scare){
        id = u.id;
        max = scare;
      }
    }
  }
  if(id != -1){
    myUnits[id].set = 1;
  }else{
    if(distId != -1 && type == WORKER_FACTORY && count == 0){
      myUnits[distId].set = 1;
    }
  }
}

// 生産するユニットの行動を考える
void thinkBuild() {
  int workerCount = 0; // 自分のワーカーの個数
  int warriorFactoryCount = 0; // 自分の拠点の個数
  int workerFactoryCount = 0; //自分の村の数
  int maxdist = 0; //もっとも遠いワーカーの距離
  int maxx = 0; //もっとも遠いワーカーのx座標
  int maxy = 0; //もっとも遠いワーカーのy座標
  vector<point> workerFactoryPoint; //村の位置
  point warriorFactoryPoint; //拠点の位置
  //int resourceNum = resourceNearMyPoint.size()+resourceNearEmenyPoint.size(); //見つけた資源の数
  int resourceNum = resourceMyPoint.v.size() + resourceFreePoint.v.size();
  //bool search; //ワーカーの視野内に敵がいるか
  // 現在のユニット数を数える
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    point p;
    if (u.type == WORKER) {
      workerCount++;
    }
    if (u.type == WARRIOR_FACTORY) {
      warriorFactoryCount++;
      warriorFactoryPoint.y = u.y;
      warriorFactoryPoint.x = u.x;
    }
    if (u.type == WORKER_FACTORY) {
      workerFactoryCount++;
      p.x = u.x;
      p.y = u.y;
      workerFactoryPoint.push_back(p);
    }
  }

  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER) {
      if(dist(myCastle.y,myCastle.x,u.y,u.x) > maxdist && warriorFactoryCount == 0){
        maxx = u.x;
        maxy = u.y;
        maxdist = dist(myCastle.y,myCastle.x,u.y,u.x);
      }
      if(warriorFactoryCount != 0){
        if(warriorFactoryPoint.y == u.y && warriorFactoryPoint.x == u.x){
          maxx = u.x;
          maxy = u.y;
        }
      }
    }

  }

  // ワーカーを生産する村を選択
  if(currentTurn%2==0 || warriorFactoryCount==0) SelectProduce(warriorFactoryCount,WORKER_FACTORY);
  //村を生産するワーカーを選択
  SelectProduce(warriorFactoryCount,WORKER);
  // 命令を設定
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++){
    Unit& u = it->second;
    if (u.type == CASTLE ){
      // ワーカーの数が一定数に満たないなら城でワーカーを作る
      if(workerCount < 15 && resourceNum==0){
        if (currentResource >= BUILD_COST[WORKER] && resourceWarrior - BUILD_COST[WORKER] >= 0) {
          currentResource -= BUILD_COST[WORKER];
          u.produce = WORKER;
          resourceWarrior -= BUILD_COST[WORKER];
        }
      }else{
        //城が攻撃されそうなら最低限の防御をする
        for(map<int, Unit>::iterator itop = opUnits.begin(); itop != opUnits.end(); itop++){
          Unit& opu = itop->second;
          if(dist(opu.y,opu.x,u.y,u.x) <= 10){
            if (currentResource >= BUILD_COST[WORKER]) {
              currentResource -= BUILD_COST[WORKER];
              u.produce = WORKER;
            }
          }
        }
      }
    }
    // 資源の上に村を作る
    if (u.type == WORKER) {
      if(u.set == 1){
        bool flag=true;
        for(auto i:workerFactoryPoint){
          if(dist(u.y,u.x,i.y,i.x) < 10) flag = false;
        }
        if(workerFactoryMap[u.y][u.x]) flag = false;
        if (currentResource >= BUILD_COST[WORKER_FACTORY] && flag) {
          if(warriorFactoryCount != 0 || resourceWarrior - BUILD_COST[WORKER_FACTORY] >= 0)
          currentResource -= BUILD_COST[WORKER_FACTORY];
          u.produce = WORKER_FACTORY;
          workerFactoryMap[u.y][u.x] = true;
          if(warriorFactoryCount == 0) resourceWarrior -= BUILD_COST[WORKER_FACTORY];
        }
        u.set = 0;
      }else if(warriorFactoryCount < 4){
          //最も遠いワーカーが拠点作成
        if (currentResource >= BUILD_COST[WARRIOR_FACTORY] && u.y == maxy && u.x == maxx ) {
          currentResource -= BUILD_COST[WARRIOR_FACTORY];
          u.produce = WARRIOR_FACTORY;
        }
      }
    }


    //村からワーカーを生成
    if(u.type == WORKER_FACTORY
       && workerCount <= (resourceNum+1) * 5
       && workerCount <= 100
       /*&& opCastle.id == -1*/){
      if (currentResource >= BUILD_COST[WORKER] && u.set == 1 ) {
        if(warriorFactoryCount != 0){
          currentResource -= BUILD_COST[WORKER];
          u.produce = WORKER;
        }else if(resourceWarrior - BUILD_COST[WORKER] >= 0){
          currentResource -= BUILD_COST[WORKER];
          u.produce = WORKER;
          resourceWarrior -= BUILD_COST[WORKER];
        }
      }
      u.set = 0;
    }


    // 拠点でナイト・ファイター・アサシンを作れる順で作る
    if (u.type == WARRIOR_FACTORY) {
      if(currentTurn%2 == 1 || opCastle.id == -1){
        int createdUnitType = 0;
        if(currentResource >= BUILD_COST[ASSASSIN]) createdUnitType = ASSASSIN;
        else if(currentResource >= BUILD_COST[FIGHTER]) createdUnitType = FIGHTER;
        else if(currentResource >= BUILD_COST[KNIGHT] /*&& opCastle.id == -1*/) createdUnitType = KNIGHT;
        if(opCastle.id != -1 && (createdUnitType != ASSASSIN || searchAssasin >= 10)){
          if(currentResource >= BUILD_COST[SearchEnemyWarrior()]) createdUnitType = SearchEnemyWarrior();
        }
        if (createdUnitType != 0) {
          currentResource -= BUILD_COST[createdUnitType];
          u.produce = createdUnitType;
        }
      }
    }
  }
}

void think() {
    // ユニットに命令を設定する
    thinkWorker();
    thinkWarrior();
    thinkBuild();

    // ユニットに設定された命令を文字列として出力する
    vector<string> outputs;
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
        Unit& u = it->second;
        string s = u.toOrderString();
        if (s.size() > 0) {
            outputs.push_back(s);
            // このサンプルコードはユニットに設定した命令がターンをまたいでも引き継がれる実装をしているため、
            // 生産の命令は次ターンのために初期化しておく。
            if (u.isProducing()) u.free();
        }
    }

    cout << outputs.size() << endl; // 出力のはじめに命令の個数を出力
    for (int i = 0; i < (int)outputs.size(); i++)
      cout << outputs[i] << endl; // 命令を一行ずつ出力
}


int main() {

    // AI の名前を出力
    cout << "SampleAI" << endl;
    cout.flush(); // 忘れずに標準出力をフラッシュする
    while (input()) { // 入力が読めない場合には false を返すのでループを抜ける
        think();
        cout.flush(); // 忘れずに標準出力をフラッシュする
    }
    return 0;
}
