#include "Status.h"
#include "skill.h"

void StoneMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 1 << " " << y << " " << x << endl;
}

void StoneRival(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (st.field[y][x].isObj()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 2 << " " << y << " " << x << endl;
}

void ThunderMe(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (!st.field[y][x].isObject()) {
    x = rand()%14;
    y = rand()%17;
  }
  cout << 3 << " " << y << " " << x << endl;
}

void ThunderRival(State st){
  int x = rand()%14;
  int y = rand()%17;
  while (!st.field[y][x].isObject()) {
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
  int id = rand()%2;
  cout << 7 << " " << id << endl;
}

int thinkSkil(State myState,State rivalState,vector<Skill>skills){
  int id = rand()%8;
  if(myState.skillPoint >= skills[id].cost && id != 6 && id == -1){
    cout << 3 << endl;
    switch(id){
    case 0:
      cout << 0 << endl;
      break;
    case 1:
      StoneMe(myState);
      break;
    case 2:
      StoneRival(rivalState);
      break;
    case 3:
      ThunderMe(myState);
      break;
    case 4:
      break;
    case 5:
      DummyMe(myState);
      break;
    case 6:
      DummyRival(rivalState);
      break;
    case 7:
      WanwanWarp(myState);
      break;
    }

    return id;
  }else{
    cout << 2 << endl;
  }
  return -1;
}
