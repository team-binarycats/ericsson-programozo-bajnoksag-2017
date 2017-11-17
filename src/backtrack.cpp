#include "main.h"
#include <bits/stdc++.h>

using namespace std;

const size_t BOARD_SIZE_X = 80;
const size_t BOARD_SIZE_Y = 100;

template <typename T>
class Value {
protected:
	T value;

public:
	typedef T TYPE;

	void set(T value) { this->value = value; }
	T get() const { return this->value; }

	operator T() const { return this->get(); }
	T& operator=(const T& other) { this->set(other); return other; }

	Value(T value) {
		this->set(value);
	}
};

template <typename T, T _MIN, T _MAX>
class RangedValue : public Value<T> {
public:
	static const T MIN = _MIN;
	static const T MAX = _MAX;

	void set(T value){
		if (value < MIN) throw domain_error("too low");
		if (value > MAX) throw domain_error("too high");

		this->value = value;
	}

	RangedValue(T value) : Value<T>(value) {}
};

typedef RangedValue<size_t, 0, BOARD_SIZE_X-1> X;
typedef RangedValue<size_t, 0, BOARD_SIZE_Y-1> Y;

struct Dir {
	X x;
	Y y;

	Dir(X x, Y y) : x(x), y(y) {}
	Dir(const ericsson2017::protocol::Direction dir) : Dir(
		( dir == ericsson2017::protocol::Direction::UP ) ? -1 : ( dir == ericsson2017::protocol::Direction::DOWN ) ? 1 : 0,
		( dir == ericsson2017::protocol::Direction::LEFT ) ? -1 : ( dir == ericsson2017::protocol::Direction::RIGHT ) ? 1 : 0
	) {}
	Dir(const ericsson2017::protocol::Enemy::Direction::Reader dir) : Dir(
		( dir.getVertical() == ericsson2017::protocol::Direction::UP ) ? -1 : ( dir.getVertical() == ericsson2017::protocol::Direction::DOWN ) ? 1 : 0,
		( dir.getHorizontal() == ericsson2017::protocol::Direction::LEFT ) ? -1 : ( dir.getHorizontal() == ericsson2017::protocol::Direction::RIGHT ) ? 1 : 0
	) {}
};

struct Pos {
	X x;
	Y y;

	Pos(X x, Y y) : x(x), y(y) {}
	Pos(const ericsson2017::protocol::Position::Reader pos) : Pos(pos.getX(), pos.getY()) {}

	template <typename T>
	operator T(){
		return x * X::MAX + y;
	}
};

typedef int Owner;

struct Cell {
	Value<Owner> owner;
	Value<bool> attackable;
	Pos pos;

	Cell(Owner owner, const bool attackable, const Pos pos) : owner(owner), attackable(attackable), pos(pos) {}
	Cell(const ericsson2017::protocol::Cell::Reader cell, const Pos pos) : Cell(cell.getOwner(), cell.getAttack().isUnit() || cell.getAttack().getCan(), pos) {}
	Cell(const pair<ericsson2017::protocol::Cell::Reader, Pos> pair) : Cell(pair.first, pair.second) {}
	Cell(const pair<Pos, ericsson2017::protocol::Cell::Reader> pair) : Cell(pair.second, pair.first) {}
};

class Cells : public array<Cell*, BOARD_SIZE_X*BOARD_SIZE_Y> {

public:
	Cells(const capnp::List<capnp::List<ericsson2017::protocol::Cell>>::Reader cells) {
		for (X::TYPE x = X::MIN; x <= X::MAX; x++) {
			for (Y::TYPE y = Y::MIN; y <= Y::MAX; y++) {
				this->at(Pos(x, y)) = new Cell(cells[x][y], Pos(x, y)); //TODO do it in initialisation
			}
		}
	}
};

struct Enemy {
	Pos pos;
	Dir dir;

	Enemy(const ericsson2017::protocol::Enemy::Reader enemy) : pos(enemy.getPosition()), dir(enemy.getDirection()) {}
};

class Enemies : public vector<Enemy*> {

public:
	Enemies(const capnp::List<ericsson2017::protocol::Enemy>::Reader enemies) {
		for (auto enemy : enemies) {
			this->push_back(new Enemy(enemy));
		}
	}
};

typedef int Health;

typedef int Killer;

struct Unit {
	Pos pos;
	Dir dir;
	Health health;
	Killer killer;

	Unit(const ericsson2017::protocol::Unit::Reader unit) : pos(unit.getPosition()), dir(unit.getDirection()), health(unit.getHealth()), killer(unit.getKiller()) {}
};

struct State {
	Value<int> level;
	Value<int> tick;
	Cells cells;
	Enemies enemies;
	Unit unit;

	bool my(Cell* cell) {
		return cell->owner == 0;
	}

	State(const ericsson2017::protocol::Response::Reader response) : level(response.getInfo().getLevel()), tick(response.getInfo().getTick()), cells(response.getCells()), enemies(response.getEnemies()), unit(response.getUnits()[0]) {}
	ericsson2017::protocol::Direction getNextDirection() {
		using namespace ericsson2017::protocol;
		switch (my(cells[unit.pos])) {
			case true:
				return rand()%2 ? Direction::LEFT : Direction::UP;

			case false:
				return rand()%2 ? Direction::RIGHT : Direction::DOWN;
		}
	}
};

_SETUP {
	(void)level; // Unused
	(void)reason; // Unused
} // Nice code, it's stateless

_MAIN_LOOP {
	State state = State(response);
	move.setDirection(state.getNextDirection());
}
