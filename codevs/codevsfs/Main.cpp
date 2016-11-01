/**
 * コンパイル： g++ -std=c++11 Main.cpp
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>
#include <time.h>
#include <vector>

using namespace std;

/** ブロックが置かれていない場所の値 */
static const int EMPTY_BLOCK = 0;

/** お邪魔ブロックの値 */
int OBSTACLE_BLOCK = 11;

mt19937 MT(8410325);

/**
 * from以上、to未満の乱数を返します。
 */
int randInt(int from, int to) {
  uniform_int_distribution<int> rand(from, to - 1);
  return rand(MT);
}

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

class Field {
public:
  int W, H;
  vector<vector<int>> blocks;
  vector<vector<bool>> moved;
  vector<vector<bool>> remove;
  int meanPos;

  Field() {}
  Field(int W, int H):
    W(W),
    H(H),
    blocks(vector<vector<int>>(H, vector<int>(W, 0))),
    meanPos(0) {}

  /**
   * フィールド1つ分を入力します。
   */
  void input() {
    int bcount = 0;
    meanPos = 0;
    blocks.clear();
    for (int i = 0; i < H; i++) {
      vector<int> row;
      for (int j = 0; j < W; j++) {
        if(i < 3){
          row.push_back(0);
        }else{
          int block;
          cin >> block;
          if(block > 0 && block != OBSTACLE_BLOCK){
            meanPos += j;
            bcount++;
          }
          row.push_back(block);
        }
      }
      blocks.push_back(row);
    }
    if(bcount != 0)
      meanPos /= bcount;
    moved.clear();
    remove.clear();
    for(int y=0;y<H;y++){
      vector<bool> row;
      for(int x=0;x<W;x++){
        row.push_back(false);
      }
      moved.push_back(row);
      remove.push_back(row);
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

  void fallPack(Pack p, int pos){
    for(int px=0;px<3;px++){
      int y = 0;
      for(int py=0;py<3;py++){
        if(p.blocks[2-py][px] == 0)
          continue;
        while(y < H && blocks[y][pos+px] == 0)
          y++;
        for(;py<3;py++){
          if(p.blocks[2-py][px] == 0)
            continue;
          blocks[y-1][pos+px] = p.blocks[2-py][px];
          moved[y-1][pos+px] = true;
          y--;
        }
      }
    }
  }

  int crearBlock(int combo){
    float comboBonus = 1.3;
    for(int i=0;i< combo;i++)
      comboBonus *= 1.3;
    int score = 0;
    vector<vector<pair<int,int>>> removed;
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        if(!moved[y][x] || blocks[y][x] == OBSTACLE_BLOCK)
          continue;
        int dx[4] = {0,1,1,1};
        int dy[4] = {1,-1,0,1};
        for(int i=0;i<4;i++){
          int nowx = x;
          int nowy = y;
          int nowSum = 0;
          vector<pair<int,int>> before;
          while(x == nowx + (int)before.size()*dx[i] && y == nowy + (int)before.size()*dy[i]){
            int sum = nowSum;
            int count = before.size();
            vector<pair<int,int>> rmBlock = before;
            while(sum < 10 && inField(nowy+count*dy[i],nowx+count*dx[i]) && blocks[nowy+count*dy[i]][nowx+count*dx[i]] != 0){
              sum += blocks[nowy+count*dy[i]][nowx+count*dx[i]];
              rmBlock.push_back(pair<int,int>(nowy+count*dy[i],nowx+count*dx[i]));
              count++;
            }
            if(sum != 10){
              nowx -= dx[i];
              nowy -= dy[i];
              if(inField(nowy,nowx) && blocks[nowy][nowx] != 0 &&nowSum+blocks[nowy][nowx] < 10){
                nowSum += blocks[nowy][nowx];
                before.push_back(pair<int,int>(nowy,nowx));
              }
              continue;
            }
            sort(rmBlock.begin(),rmBlock.end());
            bool same = false;
            for(auto itr = removed.begin();itr != removed.end();itr++){
              if(same)
                continue;
              if(*itr == rmBlock)
                same = true;
            }
            if(same){
              nowx -= dx[i];
              nowy -= dy[i];
              if(inField(nowy,nowx) && blocks[nowy][nowx] != 0 &&nowSum+blocks[nowy][nowx] < 10){
                nowSum += blocks[nowy][nowx];
                before.push_back(pair<int,int>(nowy,nowx));
              }
              continue;
            }
            if(sum == 10){
              score += count;
              for(int j=0;j<count;j++)
                remove[nowy+j*dy[i]][nowx+j*dx[i]] = true;
              removed.push_back(rmBlock);
            }
            nowx -= dx[i];
            nowy -= dy[i];
            if(inField(nowy,nowx) && blocks[nowy][nowx] != 0 &&nowSum+blocks[nowy][nowx] < 10){
              nowSum += blocks[nowy][nowx];
              before.push_back(pair<int,int>(nowy,nowx));
            }
          }
        }
      }
    }
    for(int y=0;y<H;y++){
      for(int x=0;x<W;x++){
        if(remove[y][x]){
          blocks[y][x] = 0;
        }
        remove[y][x] = false;
        moved[y][x] = false;
      }
    }
    return (int)comboBonus*(int)(score/2);
  }

  void fillBlock(){
    for(int x=0;x<W;x++){
      int nowH = 0;
      for(int y=0;y<H;y++){
        if(blocks[H-y-1][x] == 0){
          while(y<H && blocks[H-y-1][x] == 0)
            y++;
        }
        if(y != H && nowH != y){
          blocks[H-nowH-1][x] = blocks[H-y-1][x];
          blocks[H-y-1][x] = 0;
          moved[H-nowH-1][x] = true;
        }
        nowH++;
      }
    }
  }

  bool gameSet(){
    for(int y=0;y<3;y++){
      for(int x=0;x<W;x++){
        if(blocks[y][x] != 0)
          return false;
      }
    }
    return true;
  }
  bool inField(int y,int x){
    return (y >= 0 && y < H && x >= 0 && x < W);
  }

  pair<int,int> doTurn(Pack p,int pos){
    fallPack(p,pos);
    int combo = 0;
    int score = 1;
    int sum_score = 0;
    while(score > 0){
      score = crearBlock(combo);
      fillBlock();
      sum_score += score;
      combo++;
    }
    bool alive = gameSet();
    if(!alive)
      return pair<int,int>(-1,-1);
    else
      return pair<int,int>(sum_score,combo-1);
  }
};


class State {
public:
  int W, H, T, S, N;
  int turn;
  int remTime;
  vector<Pack> packs;
  Field myField;
  Field enemyField;
  int myObstacle;
  int enemyObstacle;
  vector<pair<int,int>> ans;
  int maxScore;
  int maxCombo;
  int beam;
  priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> maxScores;
  vector<vector<pair<int,int>>> bouts;

  State() :
    maxScore(-1),
    maxCombo(-1) {}

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
    bouts.clear();
    maxScores = priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>>();
    if(myObstacle > 0){
      Field f = myField;
      maxScore = -1;
      maxCombo = -1;
      int obstacle = myObstacle;
      vector<pair<int,int>> outs;
      vector<pair<int,int>> now;
      for(int i=0;i<(int)ans.size();i++){
        Pack p = packs[turn+i];
        obstacle -= p.fillWithObstacle(obstacle);
        p.rotate(ans[i].second);
        pair<int,int> sides = p.getSides();
        if(ans[i].first + sides.first < 0 || ans[i].first + sides.second > 9)
          break;
        pair<int,int> result = f.doTurn(p,ans[i].first);
        int combo = result.second;
        int score = result.first;
        obstacle -= score/5;
        if(score < 0)
          break;
        outs.push_back(ans[i]);
        if(score > maxScore){
          maxScore = score;
          maxCombo = combo;
          now = outs;
        }
      }
      ans.clear();
      ans = now;
    }
    if(maxScore < 100)
      beam = 4;
    else
      beam = 1;
    maxScores.push(pair<int,int>(maxScore,0));
    bouts.push_back(ans);
    allSearch(myField,myObstacle,vector<pair<int,int>>(),turn);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> nowScores = maxScores;
    vector<vector<pair<int,int>>> nowOuts = bouts;
    while(beam != 0){
      maxScores = nowScores;
      bouts = nowOuts;
      nowScores = priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>>();
      nowOuts.clear();
      while(!maxScores.empty()){
        int ind = maxScores.top().second;
        int score = maxScores.top().first;
        maxScores.pop();
        vector<pair<int,int>> doneList;
        int obstacle = myObstacle;
        Field nextField = myField;
        for(int depth=0;depth < (int)bouts[ind].size();depth++){
          Pack p = packs[turn+depth];
          obstacle -= p.fillWithObstacle(obstacle);
          p.rotate(bouts[ind][depth].second);
          int nowscore = nextField.doTurn(p,bouts[ind][depth].first).first;
          obstacle -= nowscore/5;
          doneList.push_back(bouts[ind][depth]);
          pair<int,vector<pair<int,int>>> mresult;
          mresult = monte(20,5,nextField,obstacle,doneList,turn+depth+1);
          if(score < nowscore){
            score = nowscore;
            bouts[ind] = doneList;
          }
          if(score < mresult.first){
            score = mresult.first;
            bouts[ind] = mresult.second;
          }
        }
        if(nowScores.size() < beam){
          nowScores.push(pair<int,int>(score,nowOuts.size()));
          nowOuts.push_back(bouts[ind]);
        }else if(nowScores.top().first < score){
          nowOuts[nowScores.top().second] = bouts[ind];
          nowScores.push(pair<int,int>(score,nowScores.top().second));
          nowScores.pop();
        }
      }
      beam--;
    }
    cerr << ans.size() << " " << maxScore << endl;
    if((int)ans.size() == 0){
      cout << 0 << " " << 0 << endl;
    }else{
      cout << ans[0].first << " " << ans[0].second << endl;
      ans.erase(ans.begin());
      if((int)ans.size() == 0){
        //cerr << maxScore << " " << turn << endl;
        maxScore = -1;
        maxCombo = -1;
      }
    }
    cout.flush();
  }

  void allSearch(Field field,int obstacle,vector<pair<int,int>> outs,int mturn){
    Pack fp = packs[mturn];
    fp.fillWithObstacle(obstacle);
    for(int rot=0;rot < 4;rot++){
      Pack p = fp;
      p.rotate(rot);
      for(int pos = -2;pos < 10;pos++){
        Field nextField = field;
        vector<pair<int,int>> mouts = outs;
        pair<int,int> sides = p.getSides();
        if(pos + sides.first < 0 || pos + sides.second > 9)
          continue;
        int score = nextField.doTurn(p,pos).first;
        if(score < 0)
          continue;
        mouts.push_back(pair<int,int>(pos,rot));
        if(maxScore < score){
          maxScore = score;
          ans = mouts;
        }
        if(maxScores.size() < beam){
          maxScores.push(pair<int,int>(score,bouts.size()));
          bouts.push_back(mouts);
        }else if(maxScores.top().first < score){
          bouts[maxScores.top().second] = mouts;
          maxScores.push(pair<int,int>(score,maxScores.top().second));
          maxScores.pop();
        }
        pair<int,vector<pair<int,int>>> mresult =  monte(50,5,nextField,obstacle,mouts,mturn+1);
        if(maxScores.size() < beam){
          maxScores.push(pair<int,int>(mresult.first,bouts.size()));
          bouts.push_back(mresult.second);
        }else if(maxScores.top().first < mresult.first){
          bouts[maxScores.top().second] = mresult.second;
          maxScores.push(pair<int,int>(mresult.first,maxScores.top().second));
          maxScores.pop();
        }
      }
    }
  }

  pair<int,vector<pair<int,int>>> monte(int sampleNum,int depthNum,Field field,int obstacle,vector<pair<int,int>> outs,int mturn){
    int nowMax = 0;
    vector<pair<int,int>> nowOuts;
    for(int sample=0;sample<sampleNum;sample++){
      Field nextField = field;
      int nextObstacle = obstacle;
      vector<pair<int,int>> mouts = outs;
      bool alive = true;
      for(int depth=0;depth<depthNum;depth++){
        if(!alive || mturn + depth > 499)
          continue;
        Pack p = packs[mturn+depth];
        nextObstacle -= p.fillWithObstacle(nextObstacle);
        int rot = randInt(0, 4);
        p.rotate(rot);
        pair<int,int> sides = p.getSides();
        int packWidth = sides.second - sides.first + 1;
        int pos = randInt(0, W - packWidth + 1) - sides.first;
        pair<int,int> result = nextField.doTurn(p,pos);
        int score = result.first;
        int combo = result.second;
        if(score < 0){
          alive = false;
          continue;
        }
        nextObstacle -= score/5;
        mouts.push_back(pair<int,int>(pos,rot));
        if(maxScore <= score){
          if(maxScore == score){
            if(ans.size() > mouts.size())
              ans = mouts;
          }else{
            ans = mouts;
            maxScore = score;
          }
        }
        if(nowMax < score){
          nowOuts = mouts;
          nowMax = score;
        }
      }
    }
    return pair<int,vector<pair<int,int>>>(nowMax,nowOuts);
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
