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
        int block;
        cin >> block;
        row.push_back(block);
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
    nextMyField = myField;
    moved.clear();
    for(int y=0;y<H;y++){
      vector<int> row;
      for(int x=0;x<W;x++){
        row.push_back(false);
      }
      moved.push_back(row);
    }
    int rot = randInt(0, 4);

    myObstacle -= packs[turn].fillWithObstacle(myObstacle);
    packs[turn].rotate(rot);

    pair<int,int> sides = packs[turn].getSides();
    int packWidth = sides.second - sides.first + 1;
    int pos = randInt(0, W - packWidth + 1) - sides.first;

    fallPack(packs[turn],pos);
    while(crearBlock() > 0)
      fillBlock();

    for(int iy=0;iy<H;iy++){
      for(int ix=0;ix<W;ix++){
        cerr << nextMyField.blocks[iy][ix];
      }
      cerr << endl;
    }
    cerr << endl;

    cout << pos << " " << packs[turn].rotateCnt << endl;
    cout.flush();
  }

  bool fallPack(Pack p, int pos){
    int btm[3]={};
    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        if(p.blocks[2-j][i] == 0 || btm[i] != 0)
          continue;
        int nowW = 0;
        while(nowW+(2-j) != H && nextMyField.blocks[nowW+(2-j)][pos+i] == 0)
          nowW++;
        if(nowW > 2)
          btm[i] = nowW-1;
        else
          return false;
      }
    }
    for(int i=0;i<3;i++){
      bool empty = false;
      int emp = 0;
      for(int j=0;j<3;j++){
        if(p.blocks[2-j][i] != 0){
          nextMyField.blocks[btm[i]+2-j+emp][pos+i] = p.blocks[2-j][i];
          moved[btm[i]+2-j+emp][pos+i] = true;
          empty = true;
        }else if(empty){
          emp++;
        }
      }
    }
    return true;
  }

  int crearBlock(){
    int score = 0;
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        if(!moved[y][x])
          continue;
        int dx[8] = {-1,0,1,-1,1,-1,0,1};
        int dy[8] = {1,1,1,0,0,-1,-1,-1};
        for(int i=0;i<8;i++){
          if(y+dy[i] < 0 || y+dy[i] >= H || x+dx[i] < 0 || x+dx[i] >= W)
            continue;
          int sum = 0;
          int count = 0;
          while(sum < 10 && y+count*dy[i] >= 0 && y+count*dy[i] < H && x+count*dx[i] >= 0 && x+count*dx[i] < W){
            sum += nextMyField.blocks[y+count*dy[i]][x+count*dx[i]];
            count++;
          }
          if(sum == 10){
            score++;
            for(int j=1;j<count;j++){
              nextMyField.blocks[y+j*dy[i]][x+j*dx[i]] = 0;
            }
          }
        }
        nextMyField.blocks[y][x] = 0;
        moved[y][x] = false;
      }
    }
    return score;
  }

  void fillBlock(){
    for(int x=0;x<W;x++){
      for(int y=0;y<H;y++){
        if(nextMyField.blocks[H-y-1][x] != 0)
          continue;
        int empty = 0;
        while(y+empty<H && nextMyField.blocks[H-y-empty-1][x] == 0){
          empty++;
        }
        int mvCnt = 0;
        for(int i=y+empty;i<H && nextMyField.blocks[H-i-1][x] != 0;i++){
          nextMyField.blocks[H-y-1][x] = nextMyField.blocks[H-i-1][x];
          moved[H-y-mvCnt-1][x] = true;
          nextMyField.blocks[H-i-1][x] = 0;
          mvCnt++;
        }
      }
    }
  }
};

int main() {
  cout << "SampleAI.cpp" << endl;
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
