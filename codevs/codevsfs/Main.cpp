/**
 * コンパイル： g++ -std=c++11 Main.cpp
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

/** ブロックが置かれていない場所の値 */
static const int EMPTY_BLOCK = 0;

/** お邪魔ブロックの値 */
int OBSTACLE_BLOCK = -10;

mt19937 MT(8410325);

/**
 * from以上、to未満の乱数を返します。
 */
int randInt(int from, int to) {
  uniform_int_distribution<int> rand(from, to - 1);
  return rand(MT);
}

class Field {
public:
  int W, H;
  vector<vector<int>> blocks;

  Field() {}
  Field(int W, int H):
    W(W),
    H(H),
    blocks(vector<vector<int>>(H, vector<int>(W, 0))) {}

  /**
   * フィールド1つ分を入力します。
   */
  void input() {
    blocks.clear();
    for (int i = 0; i < H; i++) {
      vector<int> row;
      for (int j = 0; j < W; j++) {
        if(i < 3){
          row.push_back(0);
        }else{
          int block;
          cin >> block;
          row.push_back(block);
        }
      }
      blocks.push_back(row);
    }
    string endStr;
    cin >> endStr;
  }

  /**
   * フィールドの状態を標準エラー出力します。
   */
  void show() {
    for (int i = 0; i < H; i++) {
      for (int j = 0; j < W; j++) {
        cerr << blocks[i][j] << " ";
      }
      cerr << endl;
    }
    cerr << "====" << endl;
    cerr.flush();
  }

  bool inField(int y,int x){
    return (y >= 0 && y < H && x >= 0 && x < W);
  }
};

class Pack {
public:
  int T;
  vector<vector<int>> blocks;
  int rotateCnt;

  Pack(int T, vector<vector<int>> blocks):
    T(T),
    blocks(blocks),
    rotateCnt(0) {}

  /**
   * T*Tのサイズのパックを1つ入力します。Packを生成するときは必ず呼び出してください。
   */
  static Pack inputFirst(int T) {
    vector<vector<int>> blocks;
    for (int i = 0; i < T; i++) {
      vector<int> row;
      for (int j = 0; j < T; j++) {
        int block;
        cin >> block;
        row.push_back(block);
      }
      blocks.push_back(row);
    }
    string endStr;
    cin >> endStr;
    return Pack(T, blocks);
  }

  /**
   * お邪魔ブロックでパックを埋めます。回転後の状態で呼び出さないでください。
   */
  int fillWithObstacle(int obstacleCnt) {
    int fillCnt = 0;
    for (int i = 0; i < T; i++) {
      for (int j = 0; j < T; j++) {
        if (fillCnt < obstacleCnt && blocks[i][j] == EMPTY_BLOCK) {
          blocks[i][j] = OBSTACLE_BLOCK;
          fillCnt++;
        }
        if (fillCnt >= obstacleCnt) {
          return fillCnt;
        }
      }
    }
    return fillCnt;
  }

  /**
   * このパックを90*rot度、回転させます。
   */
  void rotate(int rot) {
    rotateCnt += rot;
    rotateCnt %= 4;

    for (int r = 0; r < rot; r++) {
      vector<vector<int>> tmp = blocks;
      for (int i = 0; i < T; i++) {
        for (int j = 0; j < T; j++) {
          blocks[j][T - 1 - i] = tmp[i][j];
        }
      }
    }
  }

  /**
   * このパックの両端を削れるだけ削った、本来の両端のインデックスをpairで返します。
   * firstが左端、secondが右端のインデックス。
   */
  pair<int,int> getSides() {
    int minSide = T;
    int maxSide = 0;
    for (int i = 0; i < T; i++) {
      for (int j = 0; j < T; j++) {
        if (blocks[i][j] != EMPTY_BLOCK) {
          minSide = min(minSide, j);
          maxSide = max(maxSide, j);
        }
      }
    }
    return pair<int,int>(minSide, maxSide);
  }

  /**
   * このパックの状態を標準エラー出力します
   */
  void show() {
    for (int i = 0; i < T; i++) {
      for (int j = 0; j < T; j++) {
        cerr << blocks[i][j] << " ";
      }
      cerr << endl;
    }
    cerr.flush();
  }
};

class State {
public:
  int W, H, T, S, N;
  int turn;
  int remTime;
  vector<Pack> packs;
  vector<vector<int>> moved;
  vector<vector<int>> remove;
  Field myField;
  Field nextMyField;
  Field enemyField;
  int myObstacle;
  int enemyObstacle;

  State() {}

  /**
   * Stateを入力します。Stateを生成するときは必ず呼び出してください。
   */
  static State inputFirst() {
    State st;
    cin >> st.W >> st.H >> st.T >> st.S >> st.N;
    st.H += 3;
    st.packs.clear();
    for (int i = 0; i < st.N; i++) {
      Pack pack = Pack::inputFirst(st.T);
      st.packs.push_back(pack);
    }
    st.myField = Field(st.W, st.H);
    st.enemyField = Field(st.W, st.H);
    return st;
  }

  /**
   * 1ターン分の入力を行います。
   */
  bool inputTurn() {
    if (!(cin >> turn >> remTime)) return false;
    cin >> myObstacle;
    myField.input();
    cin >> enemyObstacle;
    enemyField.input();
    return true;
  }

  /**
   * 現在のターンのパックをどこに落とすか決定して、標準出力します。
   * 落とす位置の決め方
   * - 回転角度はランダムで決める
   * - 落下位置はランダムで決める
   * - ただし、落下位置は、左端・右端に詰められる場合は、それも考慮する(-2, -1, 8, 9にも落とせる場合は落とす)
   */
  void executeTurn() {
    moved.clear();
    for(int y=0;y<H;y++){
      vector<int> row;
      for(int x=0;x<W;x++){
        row.push_back(false);
      }
      moved.push_back(row);
    }
    remove.clear();
    for(int y=0;y<H;y++){
      vector<int> row;
      for(int x=0;x<W;x++){
        row.push_back(false);
      }
      remove.push_back(row);
    }
    bool alive = false;
    int outRot=0;
    int outPos=0;
    int maxScore = -1;
    int maxCombo = -1;
    myObstacle -= packs[turn].fillWithObstacle(myObstacle);
    bool test = true;
    for(int i=0;i<10000;i++){
      int rot;
      int pos;
      Pack p = packs[turn];
      nextMyField = myField;
      rot = randInt(0, 4);

      p.rotate(rot);

      pair<int,int> sides = p.getSides();
      int packWidth = sides.second - sides.first + 1;
      pos = randInt(0, W - packWidth + 1) - sides.first;
      fallPack(p,pos);
      int score = crearBlock();
      int sum_score = score;
      int combo = 0;
      if(score > 0)
        combo++;
      while(score > 0){
        fillBlock();
        score = crearBlock();
        sum_score += score;
        if(score > 0)
          combo++;
      }
      alive = gameSet();
      if(!alive)
        continue;
      if(maxScore < sum_score){
        test = false;
        maxScore = sum_score;
        maxCombo = combo;
        outPos = pos;
        outRot = rot;
      }
    }
    myField.show();
    if(test)
      cerr << "dead" << endl;

    cout << outPos << " " << outRot << endl;
    cout.flush();
  }

  void fallPack(Pack p, int pos){
    for(int px=0;px<3;px++){
      int y = 0;
      for(int py=0;py<3;py++){
        if(p.blocks[2-py][px] == 0)
          continue;
        while(y < H && nextMyField.blocks[y][pos+px] == 0)
          y++;
        for(;py<3;py++){
          if(p.blocks[2-py][px] == 0)
            continue;
          nextMyField.blocks[y-1][pos+px] = p.blocks[2-py][px];
          moved[y-1][pos+px] = true;
          y--;
        }
      }
    }
  }

  int crearBlock(){
    int score = 0;
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        if(!moved[y][x])
          continue;
        moved[y][x] = false;
        int dx[8] = {-1,0,1,-1,1,-1,0,1};
        int dy[8] = {1,1,1,0,0,-1,-1,-1};
        for(int i=0;i<8;i++){
          if(!nextMyField.inField(y+dy[i],x+dx[i]))
            continue;
          int sum = 0;
          int count = 0;
          while(sum < 10 && nextMyField.inField(y+count*dy[i],x+count*dx[i]) && nextMyField.blocks[y+dy[i]][x+dx[i]] != 0){
            sum += nextMyField.blocks[y+count*dy[i]][x+count*dx[i]];
            count++;
          }
          if(sum == 10){
            score++;
            for(int j=0;j<count;j++)
              remove[y+j*dy[i]][x+j*dx[i]] = true;
          }
        }
      }
    }
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        if(remove[y][x])
          nextMyField.blocks[y][x] = 0;
        remove[y][x] = false;
      }
    }
    return score;
  }

  void fillBlock(){
    for(int x=0;x<W;x++){
      int nowH = 0;
      for(int y=0;y<H;y++){
        if(nextMyField.blocks[H-y-1][x] == 0){
          while(y<H && nextMyField.blocks[H-y-1][x] == 0)
            y++;
        }
        if(y != H && nowH != y){
          nextMyField.blocks[H-nowH-1][x] = nextMyField.blocks[H-y-1][x];
          moved[H-nowH-1][x] = true;
        }
        nowH++;
      }
    }
  }

  bool gameSet(){
    for(int y=0;y<3;y++){
      for(int x=0;x<W;x++){
        if(nextMyField.blocks[y][x] != 0)
          return false;
      }
    }
    return true;
  }
};

int main() {
  cout << "ji_no_papo" << endl;
  cout.flush();

  State state = State::inputFirst();

  OBSTACLE_BLOCK = state.S + 1;

  for (int i = 0; i < state.N; i++) {
    bool inputSuccess = state.inputTurn();

    if (!inputSuccess) {
      cerr << "input Failed" << endl;
      cerr.flush();
      break;
    }

    state.executeTurn();
  }
}
