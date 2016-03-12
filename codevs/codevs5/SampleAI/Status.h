#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

const int INF = 876765346;

const char CELL_EMPTY = '_';
const char CELL_WALL ='W';
const char CELL_OBJECT = 'O';

class Point {
public:
  int x, y;
  Point() { x = y = -1; }
  Point(int x, int y): x(x), y(y) {}
  bool operator== (const Point &p) const { return x == p.x && y == p.y; }
};

class Skill {
public:
  int id, cost;
  Skill() { id = cost = -1; }
  Skill(int id, int cost): id(id), cost(cost) {}

  static Skill input(int id) {
    int cost;
    cin >> cost;
    return Skill(id, cost);
  }
};

class Cell : public Point {
public:
  char kind;
  bool containsNinja, containsSoul, containsDog;
  Cell() {
    kind = '?';
    containsNinja = containsSoul = containsDog = false;
  }
  Cell(int x, int y, char kind): Point(x, y), kind(kind) {
    containsNinja = containsSoul = containsDog = false;
  }

  bool isWall() const { return kind == CELL_WALL; }
  bool isObject() const { return kind == CELL_OBJECT; }
  bool isEmpty() const { return kind == CELL_EMPTY; }
  bool isObj() const {
    return containsDog || containsNinja || isWall() || isObject();
  }
};

class Character : public Point {
public:
  int id;
  Character() { id = -1; }
  Character(int id, int x, int y): id(id), Point(x, y) {}

  static Character input() {
    int id, x, y;
    cin >> id >> y >> x;
    return Character(id, x, y);
  }
};

class State {
public:
  int skillPoint;
  int H, W;
  vector< vector<Cell> > field;
  vector<Character> ninjas;
  vector<Character> dogs;
  vector<Point> souls;
  vector<int> skillCount;

  State() {
    skillPoint = H = W = -1;
    field.clear();
    ninjas.clear();
    dogs.clear();
    souls.clear();
    skillCount.clear();
  }

  static State input(int numOfSkills) {
    State st;

    cin >> st.skillPoint;
    cin >> st.H >> st.W;

    st.field.clear();

    for (int i = 0; i < st.H; i++) {
      vector<Cell> line;
      string s;
      cin >> s;
      for (int j = 0; j < s.size(); j++) {
        line.push_back(Cell(j, i, s[j]));
      }
      st.field.push_back(line);
    }

    int numOfNinjas;
    cin >> numOfNinjas;
    st.ninjas.clear();
    for (int i = 0; i < numOfNinjas; i++) {
      Character ninja = Character::input();
      st.ninjas.push_back(ninja);
      st.field[ninja.y][ninja.x].containsNinja = true;


    }

    int numOfDogs;
    cin >> numOfDogs;
    st.dogs.clear();
    for (int i = 0; i < numOfDogs; i++) {
      Character dog = Character::input();
      st.dogs.push_back(dog);
      st.field[dog.y][dog.x].containsDog = true;
    }

    int numOfSouls;
    cin >> numOfSouls;
    st.souls = vector<Point>();
    for (int i = 0; i < numOfSouls; i++) {
      int x, y;
      cin >> y >> x;
      st.souls.push_back(Point(x, y));
      st.field[y][x].containsSoul = true;
    }

    st.skillCount = vector<int>();
    for (int i = 0; i < numOfSkills; i++) {
      int count;
      cin >> count;
      st.skillCount.push_back(count);
    }

    return st;
  }
};

class Search : public Point {
public:
  int dist;
  Search(){}
  Search(int x, int y, int dist): Point(x, y), dist(dist) {}
};

class MinDist {
public:
  int dogDist, soulDist;
  MinDist(int dogDist, int soulDist): dogDist(dogDist), soulDist(soulDist) {}
};

struct nextState{
  vector<string> dir;
  int free;
  bool nearDogs;
  State state;
};
