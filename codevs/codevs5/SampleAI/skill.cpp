#include "skill.h"
#include "state.h"
#include "codevs.h"

void StoneMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 1 << " " << y << " " << x << endl;
}

void StoneRival(State &st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isStone()) {
    x = rand()%14;
    y = rand()%17;
  }
  st.field[y][x].Empty();
  cout << 2 << " " << y << " " << x << endl;
}

void ThunderMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (!st.field[y][x].isStone()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 3 << " " << y << " " << x << endl;
}

void ThunderRival(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (!st.field[y][x].isStone()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 4 << " " << y << " " << x << endl;
}

void DummyMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 5 << " " << y << " " << x << endl;
}

void DummyRival(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 6 << " " << y << " " << x << endl;
}

void WanwanWarp(State st){
  cout << 3 << endl;
  int id = rand()%2;
  cout << 7 << " " << id << endl;
}

int thinkSkil(State &myState,State rivalState,vector<Skill>skills){
  int id = rand()%8;
  if(myState.skillPoint >= skills[id].cost){
    switch(id){
    case 2:
      cout << 3 << endl;
      StoneRival(rivalState);
      break;
    case 3:
      cout << 3 << endl;
      ThunderMe(myState);
      break;
    case 5:
      cout << 3 << endl;
      DummyMe(myState);
      break;
    case 7:
      WanwanWarp(myState);
      break;
    default:
      cout << 2 << endl;
    }
    return id;
  }else{
    cout << 2 << endl;
  }
  return -1;
}
