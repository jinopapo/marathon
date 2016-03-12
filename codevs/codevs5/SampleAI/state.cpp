#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include "state.h"
#include "codevs.h"

using namespace std;

extern int dx[];
extern int dy[];
extern string ds[];

bool State::isMove(int x,int y,int dir){
  if(field[y][x].isWall()) return false;
  if(field[y][x].isEmpty()) return true;
  int nx = x + dx[dir];
  int ny = y + dy[dir];
  if(!field[ny][nx].isObj()) return true;
  else return false;
}

int State::searchNearDogs(){
  int count = 0;
  for(int dir=0;dir<4;dir++){
    int nx = ninjas[id].x + dx[dir];
    int ny = ninjas[id].y + dy[dir];
    if(field[ny][nx].containsDog)
      count++;
  }
  return count;
}

void State::simulateWalk(int dir){
  int nx = ninjas[id].x + dx[dir];
  int ny = ninjas[id].y + dy[dir];
  if (!isMove(nx,ny,dir)) return;

  ninjas[id].x = nx;
  ninjas[id].y = ny;

  if (field[ny][nx].containsSoul){
    skillPoint += 2;
    field[ny][nx].containsSoul = false;
    souls.erase( find(souls.begin(), souls.end(), Point(nx, ny)) );
  }

  if(field[ny][nx].isStone() && isMove(nx,ny,dir)){

    field[ninjas[id].y][ninjas[id].x].Empty();
    field[ninjas[id].y+dy[dir]][ninjas[id].x+dx[dir]].Stone();
  }
}

void State::searchDistDog(int minMap[17][14],int sid){
  queue<Search> bfs;
  int nx = ninjas[sid].x;
  int ny = ninjas[sid].y;
  bfs.push(Search(nx,ny,0));
  minMap[ny][nx] = 0;
  while(!bfs.empty()){
    Search sc = bfs.front();
    bfs.pop();
    for(int dir=0;dir<4;dir++){
      nx = sc.x + dx[dir];
      ny = sc.y + dy[dir];
      if(!field[ny][nx].isEmpty()) continue;
      if(minMap[ny][nx] < sc.dist) continue;
      minMap[ny][nx] = sc.dist+1;
      cerr << minMap[ny][nx];
      bfs.push(Search(nx, ny, sc.dist+1));
    }
  }
}

void State::dogSimulate(){
  int INF = 876765346;
  int minMap[17][14];
  for(int y=0;y<17;y++)for(int x=0;x<14;x++) minMap[y][x] = INF;
  for(int i=0;i<2;i++){
    searchDistDog(minMap,i);
  }
  /*for(int y=0;y<17;y++){
    for(int x=0;x<14;x++){
      cerr << minMap[y][x];
    }
    cerr << endl;
    }*/
}
