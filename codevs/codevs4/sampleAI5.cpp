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

/*const int MANHATTAN4[] = {0,0,0,0,1,0,0,0,0,
                          0,0,0,1,1,1,0,0,0,
                          0,0,1,1,1,1,1,0,0,
                          0,1,1,1,1,1,1,1,0,
                          1,1,1,1,1,1,1,1,1,
                          0,1,1,1,1,1,1,1,0,
                          0,0,1,1,1,1,1,0,0,
                          0,0,0,1,1,1,0,0,0,
                          0,0,0,0,1,0,0,0,0};
const int MANHATTAN10[] = {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
                           0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
                           0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
                           0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                           0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                           0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                           1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                           0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                           0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                           0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                           0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
                           0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
                           0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,};*/
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
  bool operator!=(const point& p) const{
    return x != p.x || y != p.y;
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
int unitmap[100][100]; //敵がいるマスは-1味方がいるマスは1何もいないマスは0
int workerNum[100][100] = {0}; // そのマスにワーカーが何人いるか。移動中のワーカーは目的地のマスでカウント。
vector<point> resourcePoint; //現在見えている資源のあるマスの座標郡
bool cellmap[100/CELL][100/CELL]; //セルが探索されているか
bool workerFactoryMap[100][100]; //村があるかなしか
int cellLength = 100/CELL; //セルの大きさ
int attackCell[100/CELL] = {}; //アタッカーが探索する予定のセル
bool attackCellBool[100/CELL][100/CELL] = {false}; //アタッカーが探索したセル
vector<point> resourceFreePoint; //ワーカーのいないセルの座標
vector<point> resourceNearMyPoint; //自分の陣地側の資源の位置
vector<point> resourceNearEmenyPoint; //敵の陣地側の資源の位置
vector<point> resourceEmenyPoint; //敵のワーカーのいる資源の位置
int insCellMap[100/CELL][100/CELL]; //このターンに探索を割り当てられるセル
int currentTurn; //現在のターン

void stageStart() {
    myUnits.clear();
    opUnits.clear();
    opCastle.id = -1;
    resourceNearMyPoint.clear();
    resourceNearEmenyPoint.clear();
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) resource[y][x] = -100;
    for (int y = 0; y < 100/CELL; y++) attackCell[y] = 0;
    for (int y = 0; y < 100/CELL; y++)for (int x = 0; x < 100/CELL; x++) attackCellBool[y][x] = false;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) see[y][x] = false;
    for (int y = 0; y < 100/CELL; y++) for (int x = 0; x < 100/CELL; x++) cellmap[y][x] = false;
    for (int y = 0; y < 100/CELL; y++) for (int x = 0; x < 100/CELL; x++) insCellMap[y][x] = 0;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) workerFactoryMap[y][x] = false;
}

int nextInt() {
    int x = -1;
    cin >> x;
    return x;
}



bool input() {
  for(int i=0;i<100;i++)for(int j=0;j<100;j++) unitmap[i][j]=0;
  remainingTime = nextInt();
  int currentStage = nextInt();
  currentTurn = nextInt();
  if (currentTurn == 0) {
    // ターンをまたいで維持される変数はステージが新しくなった時点で初期化を行う。
    stageStart();
    // ステージが始まったことをデバッnグ出力。
    // （クライアントで実行すると標準エラー出力は ./log/io/ 配下にログが出力される）
    cerr << "stage:" << currentStage << endl;
  }

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
      if (myUnits.count(id) > 0) u = myUnits[id];
      u.id = id;
      u.y = nextInt();
      u.x = nextInt();
      unitmap[u.y][u.x] = 1;
      u.hp = nextInt();
      u.type = nextInt();
      mp[id] = u;
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
      if (opUnits.count(id) > 0) u = opUnits[id];
      u.id = id;
      u.y = nextInt();
      u.x = nextInt();
      unitmap[u.y][u.x] = -1;
      u.hp = nextInt();
      u.type = nextInt();
      mp[id] = u;
      if (u.type == CASTLE) opCastle = u;
    }
    opUnits = mp;
  }
  {
    // 資源の位置を読み込み
    int resNum = nextInt();
    resourcePoint.clear();
    resourceFreePoint.clear();
    resourceEmenyPoint.clear();
    for (int i = 0; i < resNum; i++) {
      int y = nextInt();
      int x = nextInt();
      point p;
      bool flag=false;
      p.x = x;
      p.y = y;
      if(isTopLeft){
        if(dist(p.y,p.x,0,0) < 100){
          for(auto i:resourceNearMyPoint){
            if(p.x == i.x && p.y == i.y) flag=true;
          }
          if(!flag)resourceNearMyPoint.push_back(p);
        }else if(dist(p.y,p.x,99,99) < 100){
          for(auto i:resourceNearEmenyPoint){
            if(p.x == i.x && p.y == i.y) flag=true;
          }
          if(!flag)resourceNearEmenyPoint.push_back(p);
        }
      }else{
        if(dist(p.y,p.x,0,0) < 100){
          for(auto i:resourceNearEmenyPoint){
            if(p.x == i.x && p.y == i.y) flag=true;
          }
          if(!flag)resourceNearEmenyPoint.push_back(p);
        }else if(dist(p.y,p.x,99,99) < 100){
          for(auto i:resourceNearMyPoint){
            if(p.x == i.x && p.y == i.y) flag=true;
          }
          if(!flag)resourceNearMyPoint.push_back(p);
        }
      }
      resource[y][x] = unitmap[y][x];
      if(resource[y][x] == 0){
        resourceFreePoint.push_back(p);
      }
      if(resource[y][x] != -100){
        resourcePoint.push_back(p);
      }
      if(resource[y][x] == -1){
        resourceEmenyPoint.push_back(p);
      }
    }
    for(auto j:resourceNearEmenyPoint){
      bool flag=false;
      for(auto i:resourceEmenyPoint){
        if(j.x == i.x && j.y == i.y ) flag=true;
      }
      if(!flag && resource[j.y][j.x] == -1) resourceEmenyPoint.push_back(j);
      flag = false;
      for(auto i:resourceFreePoint){
        if(j.x != i.x && j.y != i.y && resource[j.y][j.x] == 0) resourceFreePoint.push_back(j);
      }
    }
    for(auto j:resourceNearMyPoint){
      bool flag = false;
      for(auto i:resourceEmenyPoint){
        if(j.x == i.x && j.y == i.y) flag = true;
      }
      if(!flag && resource[j.y][j.x] == -1)resourceEmenyPoint.push_back(j);
      for(auto i:resourceFreePoint){
        if(j.x != i.x && j.y != i.y && resource[j.y][j.x] == 0) resourceFreePoint.push_back(j);
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
  for(auto i:v){
    if((dist(i.y,i.x,u.y,u.x) <= 10 && workerNum[i.y][i.x] < 5) || resourceNearMyPoint.size()+resourceNearEmenyPoint.size() == 20){
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
    if(insCellMap[u.cellNum.y][u.cellNum.x] != 0){
      insCellMap[u.cellNum.y][u.cellNum.x]--;
    }
  }
}

//4近傍が空いてなかったら城に近づく
void searchCasleWorker(Unit& u){
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
  if(u.x/CELL+1 <= sx) u.cellNum.x = u.x/CELL+1;
  else u.cellNum.x = u.x/CELL;
  if(u.y/CELL+1 <= sy) u.cellNum.y = u.y/CELL+1;
  else u.cellNum.y = u.y/CELL;
  if(insCellMap[u.cellNum.y][u.cellNum.x] == 0)insCellMap[u.cellNum.y][u.cellNum.x]++;
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
  }
}

// ワーカーの行動を考える
void thinkWorker() {
  for(int i=0;i<100;i++)for(int j=0;j<100;j++)workerNum[i][j] = 0;

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
    }
  }
  // 命令を設定
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER) {
      // 目的地についているなら命令を再設定
      if (u.y == u.movetoy && u.x == u.movetox) {
        if(resource[u.y][u.x] == -100)u.free();
        else u.set = 1;
      }

      if (u.isFree()) {
        //視野に資源があれば一番近い資源に向かい、資源が全部見つかっていたら一番近い資源に向かう
        searchResorce(u,resourceFreePoint);
        if(u.movetoy == -1){
          searchResorce(u,resourcePoint);
        }
        if(u.movetoy == -1){
          searchResorce(u,resourceEmenyPoint);
        }

        if(u.isFree()){
          //セルを割り当てる
          if(u.cellNum == -1 ){
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
            //４近傍に開いてるセルがなく、城が見つかってなかったら城に近づく
            if(u.cellNum == -1 && opCastle.id == -1){
              searchCasleWorker(u);
            }
            //城が見つかっていたら探索していないセルに向かう
            if(u.cellNum == -1){
              searchCell(u);
            }
            if(u.cellNum == -1){
              goToResorce(u,resourceFreePoint);
              if(u.movetoy == -1){
                goToResorce(u,resourcePoint);
              }
              if(u.movetoy == -1){
                goToResorce(u,resourceEmenyPoint);
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

// 戦闘ユニットの行動を考える
void thinkWarrior() {
  int unitCountMap[100][100] = {};
  vector<point> pv; //拠点の座標
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == KNIGHT || u.type == FIGHTER || u.type == ASSASSIN) {
      unitCountMap[u.y][u.x]++;
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
        ty = opCastle.y;
        tx = opCastle.x;
      } else {
        // 敵の城の位置が不明なら敵の城がありそうな場所へ向かう
        int sx = -1;
        int sy = -1;
        if(cellmap[u.cellNum.y][u.cellNum.x]){
          u.cellNum.y = -1;
          u.cellNum.x = -1;
        }
        if(isTopLeft){
          if(u.cellNum == -1){
            int ccelcount = attackCell[0];
            for(int j=10;j<20;j++){
              if(attackCell[j-10] < ccelcount && !attackCellBool[cy[j]][cx[j]] && sx == -1){
                sy = cy[j];
                sx = cx[j];
                u.cellNum.x = sx;
                u.cellNum.y = sy;
                attackCell[j-10]++;
              }
            }
            if(sx == -1 && !attackCellBool[cy[0]][cx[0]]){
              sy = cy[10];
              sx = cx[10];
              u.cellNum.x = sx;
              u.cellNum.y = sy;
              attackCell[0]++;
            }
          }else{
            sx = u.cellNum.x;
            sy = u.cellNum.y;
          }
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
        }else{
          for(int j=0;j<10;j++){
            while(sy == -1 && sx == -1){
              for(int i=0;i<10;i++){
                if(!cellmap[cy[i]][cx[i]] /*&& ccel[i] < ccelcount*/){
                  //ccel[i]++;
                  sy = cy[i];
                  sx = cx[i];
                  cerr << "hoge" << endl;
                }
              }
              //ccelcount++;
            }
          }
          if(u.x/CELL-1 >= sx) u.cellNum.x = u.x/CELL-1;
          else u.cellNum.x = u.x/CELL;
          if(u.y/CELL-1 >= sy) u.cellNum.y = u.y/CELL-1;
          else u.cellNum.y = u.y/CELL;
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
      if(ty != -1 && tx != -1){
        if(unitCountMap[u.y][u.x] <= 10){
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

    if(u.type == ASSASSIN && u.set == 1){
      if(resourceEmenyPoint.size() != 0){
        if(u.cellNum == -1){
          int n = rand((int)resourceEmenyPoint.size());
          u.cellNum.x = resourceEmenyPoint[n].x;
          u.cellNum.y = resourceEmenyPoint[n].y;
        }else if(u.x == u.cellNum.x && u.y == u.cellNum.y){
          u.cellNum.x = -1;
          u.cellNum.y = -1;
        }
      }
      if(!(u.cellNum == -1)){
        if(u.cellNum.y > u.y){
          u.movetoy=u.y+1;
          u.movetox=u.x;
        }else if(u.cellNum.y < u.y){
          u.movetoy=u.y-1;
          u.movetox=u.x;
        }else{
          u.movetoy=u.y;
          if(u.cellNum.x > u.x) u.movetox=u.x+1;
          else if(u.cellNum.x < u.x) u.movetox=u.x-1;
          else u.movetox=u.x;
        }
      }
    }

    if(u.type == ASSASSIN && u.set == 0){
      if(rand(10) == 1){
        u.set = 1;
      }else{
        u.set = 2;
      }
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
    int resourceFree = 0; //ワーカーが5人いない資源の数
    int workerFactoryMaxDist = 0; //もっとも遠い村の距離
    point workerFactoryMaxPoint; //もっとも遠い村の座標
    bool workerFactoryMax = false; //もっとも遠い村がワーカーを生産し終わってたらtrue
    vector<point> workerFactoryPoint; //村の位置
    int resourceNum = resourceNearMyPoint.size()+resourceNearEmenyPoint.size(); //見つけた資源の数
    // 現在のユニット数を数える
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
      Unit& u = it->second;
      point p;
      if (u.type == WORKER) {
        workerCount++;
        if(dist(myCastle.y,myCastle.x,u.y,u.x) > maxdist){
          maxx = u.x;
          maxy = u.y;
          maxdist = dist(myCastle.y,myCastle.x,u.y,u.x);
        }
      }
      if (u.type == WARRIOR_FACTORY) {
        warriorFactoryCount++;
      }
      if (u.type == WORKER_FACTORY) {
        if(workerNum[u.y][u.x] < 5){
          resourceFree++;
        }
        if(workerFactoryMaxDist < dist(u.y,u.x,myCastle.y,myCastle.x)){
          workerFactoryMaxDist = dist(u.y,u.x,myCastle.y,myCastle.x);
          workerFactoryMaxPoint.x = u.x;
          workerFactoryMaxPoint.y = u.y;
        }
        workerFactoryCount++;
        p.x = u.x;
        p.y = u.y;
        workerFactoryPoint.push_back(p);
      }
    }

    // 命令を設定
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++){
        Unit& u = it->second;
        // ワーカーの数が一定数に満たないなら城でワーカーを作る
        if (u.type == CASTLE && workerCount < 15 && resourceNum==0){
            if (currentResource >= BUILD_COST[WORKER]) {
                currentResource -= BUILD_COST[WORKER];
                u.produce = WORKER;
            }
        }
        // 資源の上に村を作る
        if (u.type == WORKER) {
          if(u.set == 1 && !workerFactoryMap[u.y][u.x]){
            bool flag=false;
            for(auto i:workerFactoryPoint){
              if(dist(u.y,u.x,i.y,i.x) < 10) flag = true;
            }
            if (currentResource >= BUILD_COST[WORKER_FACTORY] && !flag) {
                currentResource -= BUILD_COST[WORKER_FACTORY];
                u.produce = WORKER_FACTORY;
                workerFactoryMap[u.y][u.x] = true;
                u.set = 2;
            }
          }else if(((workerFactoryCount >= 8) || opCastle.id != -1 || maxdist >= 60) && warriorFactoryCount < 4  && u.y == maxy && u.x == maxx && ((maxx > 30 && maxy > 30) || opCastle.id != -1 || maxdist >= 60) /*currentResource >= BUILD_COST[WARRIOR_FACTORY] && u.y == maxy && u.x == maxx*/){
            //最も遠いワーカーが拠点作成
            if (currentResource >= BUILD_COST[WARRIOR_FACTORY]) {
              currentResource -= BUILD_COST[WARRIOR_FACTORY];
              u.produce = WARRIOR_FACTORY;
            }
          }
        }


        //村からワーカーを生成
        if(u.type == WORKER_FACTORY
           && workerCount <= (resourceNum+1) * 5
           && workerCount <= 100
           && (warriorFactoryCount == 0 || currentTurn%2 == 0)){
          if(warriorFactoryCount == 0){
            if(resourceNum <= 8){
              if(workerFactoryMax){
                if (currentResource >= BUILD_COST[WORKER]) {
                  currentResource -= BUILD_COST[WORKER];
                  u.produce = WORKER;
                }
              }else{
                if(u.x == workerFactoryMaxPoint.x && u.y == workerFactoryMaxPoint.y){
                  if (currentResource >= BUILD_COST[WORKER]) {
                    currentResource -= BUILD_COST[WORKER];
                    u.produce = WORKER;
                    workerFactoryMax = true;
                  }
                }else if(currentResource >= 2*BUILD_COST[WORKER] && workerNum[u.y][u.x] <= 5 && rand(10)%2 == 1){
                  if (currentResource >= BUILD_COST[WORKER]) {
                    currentResource -= BUILD_COST[WORKER];
                    u.produce = WORKER;
                  }
                }
              }
            }
          }else{
            if(workerFactoryMax){
              if (currentResource >= BUILD_COST[WORKER]) {
                currentResource -= BUILD_COST[WORKER];
                u.produce = WORKER;
              }
            }else{
              if(u.x == workerFactoryMaxPoint.x && u.y == workerFactoryMaxPoint.y){
                if (currentResource >= BUILD_COST[WORKER]) {
                  currentResource -= BUILD_COST[WORKER];
                  u.produce = WORKER;
                  workerFactoryMax = true;
                }
              }else if(currentResource >= 2*BUILD_COST[WORKER] && workerNum[u.y][u.x] <= 5 && rand(10)%2 == 1){
                if (currentResource >= BUILD_COST[WORKER]) {
                  currentResource -= BUILD_COST[WORKER];
                  u.produce = WORKER;
                }
              }
            }
          }
        }


        // 拠点でナイト・ファイター・アサシンをランダムに作る
        if (u.type == WARRIOR_FACTORY && currentTurn%2 == 1) {
          int createdUnitType = 0;
          if(currentResource >= BUILD_COST[ASSASSIN]) createdUnitType = ASSASSIN;
          else if(currentResource >= BUILD_COST[FIGHTER]) createdUnitType = FIGHTER;
          else if(currentResource >= BUILD_COST[KNIGHT] && opCastle.id == -1) createdUnitType = KNIGHT;
          if (createdUnitType != 0) {
            currentResource -= BUILD_COST[createdUnitType];
            u.produce = createdUnitType;
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
