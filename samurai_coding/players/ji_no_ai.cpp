#include "players.hpp"
#include <tuple>
#include <vector>
#include <random>
#include <algorithm>

struct JinoAI: Player {
  default_random_engine generator;
  const int maskx[9] = {-1,0,1,-1,0,1,-1,0,1};
  const int masky[9] = {1,1,1,0,0,0,-1,-1,-1};
  const int nexty[4] = {1,0,-1,0};
  const int nextx[4] = {0,1,0,-1};
  const int weaponsAreaX[3][7] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 2, 0, 0},
    {-1,-1,-1,0, 1, 1, 1}};
  const int weaponsAreaY[3][7] = {
    {1, 2, 3, 4, 0, 0, 0},
    {1, 2, 0, 1, 0, 0, 0},
    {-1,0,1, 1, 1,-1, 0}};
  const int dangerousCost = 50;
  const int killCost = 80;

  bool inField(int x,int y,GameInfo& info){
    if(x >= 0 && x < info.width && y >= 0 && y < info.height){
      return true;
    }else{
      return false;
    }
  }

  bool isMove(SamuraiInfo& me,GameInfo& info,int x, int y){
    if(!inField(x,y,info)) return false;
    if(me.hidden == 0) return true;
    if(info.field[x+y*info.width] >= 3) return false;
    return true;
  }

  int dangerousArea(GameInfo& info,int x,int y){
    for(int si=3;si<6;si++){
      int sx = info.samuraiInfo[si].curX;
      int sy = info.samuraiInfo[si].curY;
      if(sx == -1 && sy == -1){
        for(int i=-5;i<=5;i++){
          for(int j=-abs(5-i);j<=abs(5-i);j++){
            if(isHidden(info.samuraiInfo[si],info,x+j,y+i,si)){
              for(int md = 0;md < 4;md++){
                int nowX = x+j + nextx[md];
                int nowY = y+i + nexty[md];
                for(int wd = 0;wd < 4;wd++){
                  for(int k=0;k<7;k++){
                    int rx,ry;
                    rotate(wd,weaponsAreaX[si-3][k],weaponsAreaY[si-3][k],rx,ry);
                    int wx = nowX+rx;
                    int wy = nowY+ry;
                    if(x == wx && y == wy){
                      return dangerousCost;
                    }
                  }
                }
              }
            }
          }
        }
        sx = info.samuraiInfo[si].beforeX;
        sy = info.samuraiInfo[si].beforeY;
      }
      if(sx == -1 && sy == -1) continue;
      for(int md = 0;md < 4;md++){
        int nowX = sx + nextx[md];
        int nowY = sy + nexty[md];
        for(int wd = 0;wd < 4;wd++){
          for(int i=0;i<7;i++){
            int rx,ry;
            rotate(wd,weaponsAreaX[si-3][i],weaponsAreaY[si-3][i],rx,ry);
            int wx = nowX+rx;
            int wy = nowY+ry;
            if(x == wx && y == wy){
              return dangerousCost;
            }
          }
        }
      }
    }
    return 0;
  }

  bool diffMap(GameInfo info,int x,int y,vector<tuple<int,int>> diff,int weapon){
    for(int wd = 0;wd < 4;wd++){
      bool flag = true;
      bool diff_flag = false;
      for(int i=0;i<7;i++){
        int rx,ry;
        rotate(wd,weaponsAreaX[weapon-3][i],weaponsAreaY[weapon-3][i],rx,ry);
        int ex = x+rx;
        int ey = y+ry;
        tuple<int,int> t;
        get<0>(t) = ex;
        get<1>(t) = ey;
        if(inField(ex,ey,info) && info.field[ex+ey*info.width] < 3){
          flag = false;
          break;
        }
        if(find(diff.begin(),diff.end(),t) != diff.end()){
          diff_flag = true;
        }
      }
      if(flag && diff_flag) return true;
    }
    return false;
  }

  bool isHidden(SamuraiInfo samurai,GameInfo info,int wx,int wy,int weapon){
    if(samurai.curX == -1 && samurai.curY == -1) return false;
    int x = samurai.beforeX;
    int y = samurai.beforeY;
    if(info.field[wx+wy*info.width] < 3) return false;
    if(diffMap(info,x,y,info.diff,weapon)) return true;
    for(int i=0;i<4;i++){
      if(diffMap(info,x+nextx[i],y+nexty[i],info.diff,weapon)) return true;
    }
    return false;
  }

  int attackArea(SamuraiInfo& me,GameInfo& info,int direction,int x,int y){
    int score=0;
    for(int i=0;i<7;i++){
      int rx,ry;
      if(weaponsAreaX[info.weapon][i] == 0 && weaponsAreaY[info.weapon][i] == 0) continue;
      rotate(direction,weaponsAreaX[info.weapon][i],weaponsAreaY[info.weapon][i],rx,ry);
      int wx = x+rx;
      int wy = y+ry;
      if(inField(wx,wy,info)){
        int f = info.field[wx+wy*info.width];
        if(f >= 3){
          score++;
        }
        for(int j=3;j<6;j++){
          SamuraiInfo samurai = info.samuraiInfo[j];
          if(samurai.curX == wx && samurai.curY == wy && (samurai.homeX != wx || samurai.homeY != wy)){
            score += killCost;
          }
          if(isHidden(samurai,info,wx,wy,j)){
            score += killCost/2;
          }
        }
      }
    }
    return score;
  }

  int fearArea(SamuraiInfo& me,GameInfo& info,int direction,int x,int y){
    int score=0;
    for(int ix=-2;ix<=2;ix++){
      for(int iy=0;iy<5;iy++){
        int rx;
        int ry;
        rotate(direction,ix,iy,rx,ry);
        int mx = x+rx;
        int my = y+ry;
        if(inField(mx,my,info)){
          int f = info.field[mx+my*info.width];
          if(f >= 3){
            score++;
          }
          if(f == 8){
            score++;
          }
          for(int j=0;j<6;j++){
            SamuraiInfo samurai = info.samuraiInfo[j];
            if(samurai.curX == mx && samurai.curY == my){
              if(j < 3){
                score -= 3;
              }else{
                score += 3;
              }
            }
          }
        }
      }
    }
    return score/2;
  }

  vector<int> attackAction(SamuraiInfo& me,GameInfo& info,int power){
    int maxChange=0;
    vector<int> actions;
    int action;
    int move=-1;
    for(int j=0;j<4;j++){
      int change=attackArea(me,info,j,me.curX,me.curY);
      if(maxChange < change) action = j+1;
      maxChange = max(maxChange,change);
    }
    if(power == 7){
      for(int m=0;m<4;m++){
        int x = me.curX+nextx[m];
        int y = me.curY+nexty[m];
        if(!inField(x,y,info)) continue;
        for(int j=0;j<4;j++){
          int change=attackArea(me,info,j,x,y);
          if(maxChange < change /*&& change >= killCost*/){
            action = j+1;
            move = m+5;
          }
          maxChange = max(maxChange,change);
        }
      }
    }
    if(move != -1){
      actions.push_back(move);
    }
    actions.push_back(action);
    return actions;
  }

  int moveAction(SamuraiInfo& me,GameInfo& info){
    int action=-1;
    int maxChange=-1000;
    for(int i=0;i<4;i++){
      int nowx = me.curX+nextx[i];
      int nowy = me.curY+nexty[i];
      if(!isMove(me,info,nowx,nowy))continue;
      for(int j=0;j<4;j++){
        int attackScore = attackArea(me,info,j,nowx,nowy);
        int change = attackScore + fearArea(me,info,i,nowx,nowy) - dangerousArea(info,nowx,nowy);
        if(maxChange < change) action = i+5;
        maxChange = max(maxChange,change);
      }
    }
    return action;
  }

  bool attack(SamuraiInfo& me,GameInfo& info){
    int free = 0;
    for(int i=0;i<4;i++){
      free = max(free,attackArea(me,info,i,me.curX,me.curY));
    }
    if(free >= 3){
      return true;
    }else{
      return false;
    }
  }

  void play(GameInfo& info) {
    int power = 7;
    SamuraiInfo me = info.samuraiInfo[info.weapon];
    static const int required[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1, 1};
    while(power > 1){
      int action = 0;
      if(attack(me,info) && power >= 5){
        vector<int> actions;
        actions = attackAction(me,info,power);
        if(me.hidden){
          power -= required[10];
          info.doAction(10);
        }
        for(auto a:actions){
          action = a;
          power -= required[action];
          info.doAction(action);
        }
        if(me.curX != me.homeX && me.curY != me.homeY && power >= 1){
          power -= required[9];
          info.doAction(9);
        }
      }else if(power >= 2){
        action = moveAction(me,info);
        power -= required[action];
        info.doAction(action);
        me.curX += nextx[action-5];
        me.curY += nexty[action-5];
      }
    }
    if(info.field[me.curX+me.curY*info.width] < 3 && power == 1){
      power -= required[9];
      info.doAction(9);
    }
  }
};

Player* player = new JinoAI();
