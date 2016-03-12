#include<iostream>
#include<vector>
#include<stack>
#include<queue>
#include"codevs.h"
#include"state.h"

using namespace std;

//const int INF = 876765346;

int remTime;
vector<Skill> skills;
State myState;
State rivalState;

int dx[] =    {  0,   1,   0,  -1,   0};
int dy[] =    { -1,   0,   1,   0,   0};
string ds[] = {"U", "R", "D", "L",  ""};

void think(){
  cout << 2 << endl;
  queue<State> bfs;
  vector<State> next;
  bfs.push(myState);
  if(!myState.searchNearDogs()){
    myState.id=1;
    bfs.push(myState);
  }
  while(!bfs.empty()){
    State st = bfs.front();
    bfs.pop();
    int nx = st.ninjas[st.id].x;
    int ny = st.ninjas[st.id].y;
    if(st.id == 1 && !st.searchNearDogs() && !st.field[ny][nx].containsDog){
      next.push_back(st);
    }
    if(st.ninjas[st.id].dir.size() < 2){
      for(int dir=0;dir<4;dir++){
        nx = st.ninjas[st.id].x + dx[dir];
        ny = st.ninjas[st.id].y + dy[dir];
        State ist = st;
        if(ist.field[ny][nx].isWall()) continue;
        if(!ist.isMove(nx,ny,dir)) continue;
        if(ist.field[ny][nx].containsDog) continue;
        ist.simulateWalk(dir);
        int dogs = ist.searchNearDogs();
        ist.ninjas[ist.id].dir.push_back(ds[dir]);
        if(ist.ninjas[ist.id].dir.size() <= 2)
          bfs.push(ist);
        if(ist.id == 0 && !dogs){
          ist.id = 1;
          bfs.push(ist);
        }
      }
    }
  }
  cerr << next.size() << endl;
  myState.dogSimulate();
  if(next.size()){
    State st = next[rand()%next.size()];
    for(int i=0;i<2;i++){
      for(auto s:st.ninjas[i].dir){
        cout << s;
      }
      cout << endl;
    }
  }else{
    cout << endl;
    cout << endl;
  }
}

bool input() {
  if (!(cin >> remTime)) return false;
  int numOfSkills;
  cin >> numOfSkills;
  skills.clear();
  for (int i = 0; i < numOfSkills; i++) {
    skills.push_back(Skill::input(i));
  }
  myState = State::input(skills.size());
  myState.id = 0;
  rivalState = State::input(skills.size());
  return true;
}


int main(){
  cout << "ji_no_papo" << endl;
  cout.flush();
  while(input()){
    think();
    cout.flush();
  }
  return 0;
}
