#ifndef CODEVS_H
#define CODEVS_H

#include <iostream>
#include <vector>

using namespace std;

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
  const char CELL_EMPTY = '_';
  const char CELL_WALL ='W';
  const char CELL_STONE = 'O';
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
  bool isStone() const { return kind == CELL_STONE; }
  bool isEmpty() const { return kind == CELL_EMPTY; }
  void Empty() {kind = CELL_EMPTY;};
  void Stone() {kind = CELL_STONE;};
  bool isObj() const {
    return containsDog || containsNinja || isWall() || isStone();
  }
};

class Character : public Point {
public:
  int id;
  vector<string> dir;
  Character() { id = -1; }
  Character(int id, int x, int y): id(id), Point(x, y) {}

  bool operator==( const Character& r ) const {
    return x == r.x && y == r.y;
  }

  static Character input() {
    int id, x, y;
    cin >> id >> y >> x;
    return Character(id, x, y);
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


#endif
