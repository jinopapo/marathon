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

int State::searchNearFree(int sid){
  int count = 0;
  for(int dir=0;dir<4;dir++){
    int nx = ninjas[sid].x + dx[dir];
    int ny = ninjas[sid].y + dy[dir];
    if(!field[ny][nx].containsDog && isMove(nx,ny,dir))
      count++;
  }
  return count;
}

void State::simulateWalk(int dir){
  int nx = ninjas[id].x + dx[dir];
  int ny = ninjas[id].y + dy[dir];
  if (!isMove(nx,ny,dir)) return;

  field[ninjas[id].y][ninjas[id].x].containsNinja = false;
  field[ny][nx].containsNinja = true;
  ninjas[id].x = nx;
  ninjas[id].y = ny;


  if (field[ny][nx].containsSoul){
    skillPoint += 2;
    field[ny][nx].containsSoul = false;
    auto iter = find(souls.begin(), souls.end(), Point(nx, ny));
    getSouls.push_back(iter - souls.begin());
    souls.erase(iter);
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
      if(minMap[ny][nx] <= sc.dist) continue;
      minMap[ny][nx] = sc.dist+1;
      bfs.push(Search(nx, ny, sc.dist+1));
    }
  }
}

bool compId(Character a,Character b){
  return a.id < b.id;
}

void State::dogSimulate(){
  int ddx[] = {0,-1,1,0};
  int ddy[] = {-1,0,0,1};
  struct  ndog{
    Character dog;
    int dist;
    bool operator<( const ndog& right ) const {
      return dist == right.dist ? dog.id < right.dog.id : dist < right.dist;
    }
  };
  int INF = 876765346;
  int minMap[17][14];
  vector<ndog> next;
  for(int y=0;y<17;y++)for(int x=0;x<14;x++) minMap[y][x] = INF;
  for(int i=0;i<2;i++){
    searchDistDog(minMap,i);
  }
  for(auto dog:dogs){
    ndog nd;
    nd.dog = dog;
    nd.dist = minMap[dog.y][dog.x];
    next.push_back(nd);
  }
  sort(next.begin(),next.end());
  dogs.clear();
  for(auto dog:next){
    bool flag = false;
    for(int dir=0;dir<4;dir++){
      int ny = dog.dog.y+ddy[dir];
      int nx = dog.dog.x+ddx[dir];
      if(minMap[dog.dog.y][dog.dog.x] > minMap[ny][nx] && !field[ny][nx].containsDog && !flag){
        field[ny][nx].containsDog = true;
        field[dog.dog.y][dog.dog.x].containsDog = false;
        dog.dog.x = nx;
        dog.dog.y = ny;
        flag = true;
      }
    }
    dogs.push_back(dog.dog);
  }
  sort(dogs.begin(),dogs.end(),compId);
}
