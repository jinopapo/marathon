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

const int MANHATTAN4[] = {0,0,0,0,1,0,0,0,0,
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
                           0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,};
const int dx[] = {-1,0,1,0};
const int dy[] = {0,-1,0,1};
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
  bool set;
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
vector<point> resourcePoint; //資源のあるマスの座標郡
bool cellmap[100/CELL][100/CELL]; //セルが探索されているか
bool workerFactoryMap[100][100]; //村があるかなしか
int cellLength = 100/CELL;

void stageStart() {
    myUnits.clear();
    opUnits.clear();
    opCastle.id = -1;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) resource[y][x] = -100;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) see[y][x] = false;
    for (int y = 0; y < 100; y++) for (int x = 0; x < 100; x++) cellmap[y][x] = false;
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
  int currentTurn = nextInt();
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
      }
      else if(u.type == WORKER_FACTORY) workerFactoryMap[u.y][u.x] = true;
      // 一度でも視界に入ったことがあるマスを更新
      int view = VIEW_RANGE[u.type];
      for (int sy = u.y - view; sy <= u.y + view; sy++)
        for (int sx = u.x - view; sx <= u.x + view; sx++)
          if (0 <= sy && sy < 100 && 0 <= sx && sx < 100 && dist(u.y, u.x, sy, sx) <= view)
            see[sy][sx] = true;
    }
    // 死んだユニットをリストから除くため新しい map に追加して、置き換える。
    myUnits = mp;
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
    for (int i = 0; i < resNum; i++) {
      int y = nextInt();
      int x = nextInt();
      point p;
      p.x = x;
      p.y = y;
      resource[y][x] = unitmap[y][x];
      if(resource[y][x] != -100){
        resourcePoint.push_back(p);
      }
    }
  }

  string end;
  cin >> end;
  if ("END" == end) return true;
  return false;
}

// ワーカーの行動を考える
void thinkWorker() {
  for(int i=0;i<100;i++)for(int j=0;j<100;j++)workerNum[i][j] = 0;
  struct con{
    int x;
    int y;
    int id;
  };

  vector<con> v;
  con c;
  // 現状、マスごとにワーカーが何人いるかを数える。
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER){
      if(u.isFree()){
        workerNum[u.y][u.x]++;
        c.x = u.x;
        c.y = u.y;
        c.id = u.id;
        v.push_back(c);
      }
      else{
        workerNum[u.movetoy][u.movetox]++;
        if(workerNum[u.movetoy][u.movetox] > 5){
          u.free();
          workerNum[u.movetoy][u.movetox]--;
        }
      }
    }
  }

  //セルを割り当てる
  point start;
  point end;
  point p;
  int mx,my;
  int smx,smy;
  int count=0;
  if(isTopLeft){p.x = 0; p.y = 0;smx = mx = 1; smy = my = -1;end.x = cellLength-1;end.y = cellLength-1;}
  else{p.x = cellLength-1; p.y = cellLength-1;smx = mx = 1; smy = my = -1;end.x = 0;end.y = 0;}
  if(!cellmap[p.y][p.x]){
    int min = 100;
    con a;
    for(auto i:v){
      if(min > dist(p.y,p.x,i.y/CELL,i.x/CELL) && myUnits[a.id].cellNum == -1){
        min = dist(p.y,p.x,i.y/CELL,i.x/CELL);
        a = i;
      }
    }
    if(min != 100)myUnits[a.id].cellNum = p;
    cellmap[p.y][p.x] = true;
  }
  start = p;
  while(end != p && count < (int)v.size()){
    if(start.x == p.x && start.y == p.y){mx = -mx; my = -my;p.x -= mx;}
    else{
      if(start.x == p.x && mx != smx){mx = -mx; my = -my;p.y -= my;}
      else if(start.y == p.y && p.x != start.x-mx && mx == smx){mx = -mx; my = -my;p.x -= mx;}
      else if(end.x == p.x && mx == smx){mx = -mx; my = -my;p.y += my;}
      else if(end.y == p.y && mx != smx){mx = -mx; my = -my;p.x += mx;}
      else{
        if(p.x+mx  > -1 && p.x+mx < cellLength) p.x += mx;
        if(p.y+my  > -1 && p.y+my < cellLength) p.y += my;
      }
    }
    //if(!cellmap[p.y][p.x]){u.cellNum = p;cellmap[p.y][p.x] = true;}
    if(!cellmap[p.y][p.x]){
      int min = 100;
      con a;
      for(auto i:v){
        if(min > dist(p.y,p.x,i.y/CELL,i.x/CELL) && myUnits[a.id].cellNum == -1){
          min = dist(p.y,p.x,i.y/CELL,i.x/CELL);
          a = i;
        }
      }
      if(min != 100){
        myUnits[a.id].cellNum = p;
        cellmap[p.y][p.x] = true;
        count++;
      }
    }
  }




  // 命令を設定
  for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
    Unit& u = it->second;
    if (u.type == WORKER) {
      cerr << u.cellNum.y << " " << u.cellNum.x << endl;
      // 目的地についているなら命令を再設定
      if (u.y == u.movetoy && u.x == u.movetox) {
        if(resource[u.y][u.x] == -100)u.free();
        else u.set = 1;
      }
      // 発掘するワーカがたりなかったらそこにとどまる
      /*if(workerNum[u.y][u.x] <= 5){
        u.set = 1;
        cerr << "hoge";
        }*/

      if (u.isFree()) {
        //視野に資源があれば一番近い資源に向かう
        for(auto i:resourcePoint){
          if(dist(i.y,i.x,u.y,u.x) <= 4 && workerNum[i.y][i.x] < 5){
            u.movetoy = i.y;
            u.movetox = i.x;
            workerNum[u.movetoy][u.movetox]++;
            workerNum[u.y][u.x]--;
            cellmap[u.cellNum.y][u.cellNum.x] = false;
          }
        }
        if(u.isFree()){
          /*if(u.cellNum == -1){
            // 探索するセルを割り当てる;
            point start;
            point end;
            point p;
            int mx,my;
            int smx,smy;
            if(isTopLeft){p.x = 0; p.y = 0;smx = mx = 1; smy = my = -1;end.x = cellLength-1;end.y = cellLength-1;}
            else{p.x = cellLength-1; p.y = cellLength-1;smx = mx = 1; smy = my = -1;end.x = 0;end.y = 0;}
            if(!cellmap[p.y][p.x]){u.cellNum = p;cellmap[p.y][p.x] = true;}
            start = p;
            while(u.cellNum == -1 && end != p){
              if(start.x == p.x && start.y == p.y){mx = -mx; my = -my;p.x -= mx;}
              else{
                if(start.x == p.x && mx != smx){mx = -mx; my = -my;p.y -= my;}
                else if(start.y == p.y && p.x != start.x-mx && mx == smx){mx = -mx; my = -my;p.x -= mx;}
                else if(end.x == p.x && mx == smx){mx = -mx; my = -my;p.y += my;}
                else if(end.y == p.y && mx != smx){mx = -mx; my = -my;p.x += mx;}
                else{
                  if(p.x+mx  > -1 && p.x+mx < cellLength) p.x += mx;
                  if(p.y+my  > -1 && p.y+my < cellLength) p.y += my;
                }
              }
              cerr << p.y << " " << p.x << endl;
              if(!cellmap[p.y][p.x]){u.cellNum = p;cellmap[p.y][p.x] = true;}
            }
            cerr << u.cellNum.y << " " << u.cellNum.x << endl;
            cerr << endl;
            workerNum[u.y][u.x]--;
            }*/
          if(u.cellNum.x*CELL + CELL > u.x && u.cellNum.x*CELL <= u.x && u.cellNum.y*CELL+CELL > u.y && u.cellNum.y*CELL <= u.y){
            //セル内を探索する
            for(int i=0;i<CELL;i++){
              for(int j=0;j<CELL;j++){
                if(!see[u.cellNum.y*CELL+i][u.cellNum.x*CELL+j] && u.movetoy == -1){
                  if(Math_abs(u.x - (u.cellNum.x*CELL+j)) > Math_abs(u.y - (u.cellNum.y*CELL+i))){
                    if(u.x > u.cellNum.x*CELL+j) u.movetox = u.x-1;
                    else u.movetox = u.x+1;
                    u.movetoy = u.y;
                  }
                  else{
                    if(u.y > u.cellNum.y*CELL+i) u.movetoy = u.y-1;
                    else u.movetoy = u.y+1;
                    u.movetox = u.x;
                  }
                }
                if(i == CELL-1 && j == CELL-1 && u.movetoy == -1){
                  point p1;
                  p1.x = -1;
                  p1.y = -1;
                  u.cellNum = p1;
                }
              }
            }
          }else{
            //セルを探索する
            if(Math_abs(u.x - u.cellNum.x*CELL) > Math_abs(u.y - u.cellNum.y*CELL)){
              if(u.x > u.cellNum.x*CELL) u.movetox = u.x-1;
              else u.movetox = u.x+1;
              u.movetoy = u.y;
            }
            else{
              if(u.y > u.cellNum.y*CELL) u.movetoy = u.y-1;
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
    // 命令を設定
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
        Unit& u = it->second;

        if (u.type == KNIGHT || u.type == FIGHTER || u.type == ASSASSIN) {
            // 敵の城へ向かう
            int ty = -1, tx = -1;
            if (opCastle.id != -1) {
                ty = opCastle.y;
                tx = opCastle.x;
            } else {
                // 敵の城の位置が不明なら敵の城がありそうな場所へ向かう
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
            u.movetoy = ty;
            u.movetox = tx;
        }
    }
}

// 生産するユニットの行動を考える
void thinkBuild() {
    int workerCount = 0; // 自分のワーカーの個数
    int warriorFactoryCount = 0; // 自分の拠点の個数
    int workerFactoryCount = 0; //自分の村の数
    int maxdist = 0; //
    int maxx = 0; //
    int maxy = 0; //
    // 現在のユニット数を数える
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
      Unit& u = it->second;
      if (u.type == WORKER) {
        workerCount++;
        if(dist(myCastle.y,myCastle.x,u.y,u.x) > maxdist){
          maxx = u.x;
          maxy = u.y;
        }
      }
      if (u.type == WARRIOR_FACTORY) {
        warriorFactoryCount++;
      }
      if (u.type == WORKER_FACTORY) {
        workerFactoryCount++;
      }
    }

    // 命令を設定
    for(map<int, Unit>::iterator it = myUnits.begin(); it != myUnits.end(); it++) {
        Unit& u = it->second;
        // ワーカーの数が一定数に満たないなら城でワーカーを作る
        /*if (u.type == CASTLE && workerCount < 100) {
            if (currentResource >= BUILD_COST[WORKER]) {
                currentResource -= BUILD_COST[WORKER];
                u.produce = WORKER;
            }
            }*/
        // 資源の上に村を作る
        if (u.type == WORKER) {
          if(u.set == 1 && !workerFactoryMap[u.y][u.x]){
            if (currentResource >= BUILD_COST[WORKER_FACTORY]) {
              currentResource -= BUILD_COST[WORKER_FACTORY];
              u.produce = WORKER_FACTORY;
              u.set = 2;
            }
          }else if(workerFactoryCount >= 8 && warriorFactoryCount < 4  &&u.y == maxy && u.x == maxx){
            //最も遠いワーカーが拠点作成
            if (currentResource >= BUILD_COST[WARRIOR_FACTORY]) {
              currentResource -= BUILD_COST[WARRIOR_FACTORY];
              u.produce = WARRIOR_FACTORY;
            }
          }
        }


        //村からワーカーを生成
        if(u.type == WORKER_FACTORY && workerCount <= (workerFactoryCount+1) * 5){
          if (currentResource >= BUILD_COST[WORKER]) {
            currentResource -= BUILD_COST[WORKER];
            u.produce = WORKER;
          }
        }


        // 拠点でナイト・ファイター・アサシンをランダムに作る
        if (u.type == WARRIOR_FACTORY) {
            int createdUnitType = KNIGHT + rand(3);
            if (currentResource >= BUILD_COST[createdUnitType]) {
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
