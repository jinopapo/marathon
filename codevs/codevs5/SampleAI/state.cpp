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

bool State::isClose(){
  queue<Search> bfs;
  for(int i=0;i<2;i++){
    id = i;
    int maxDist = 0;
    bool done[17][14] = {{false}};
    int nx = ninjas[i].x;
    int ny = ninjas[i].y;
    bfs.push(Search(nx,ny,0));
    while(!bfs.empty()){
      Search sc = bfs.front();
      if(maxDist < sc.dist) maxDist = sc.dist;
      bfs.pop();
      if(sc.dist >= 10) continue;
      for(int dir=0;dir<4;dir++){
        nx = sc.x + dx[dir];
        ny = sc.y + dy[dir];
        if(field[ny][nx].containsDog) continue;
        if(!isMove(nx,ny,dir)) continue;
        if(done[ny][nx]) continue;
        done[ny][nx] = true;
        bfs.push(Search(nx, ny, sc.dist+1));
      }
    }
    if(maxDist < 10) return true;
  }
  return false;
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
    int minDir = -1;
    int minDist = INF;
    for(int dir=0;dir<4;dir++){
      int ny = dog.dog.y+ddy[dir];
      int nx = dog.dog.x+ddx[dir];
      if(field[ny][nx].containsDog)continue;
      if(minDist > minMap[ny][nx]){
        minDist = minMap[ny][nx];
      }
    }
    if(minDir != -1){
      field[dog.dog.y+ddy[minDir]][dog.dog.x+ddx[minDir]].containsDog = true;
      field[dog.dog.y][dog.dog.x].containsDog = false;
      dog.dog.x += ddx[minDir];
      dog.dog.y += ddy[minDir];
    }
    dogs.push_back(dog.dog);
  }
  sort(dogs.begin(),dogs.end(),compId);
}
