#include "main.h"
#include "interface.h"
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

#define copy_call(obj, func) ([&](){ \
	auto copy = obj; \
	copy.func; \
	return copy; \
}())

struct Dir {
	Value<int> x;
	Value<int> y;

	void turn(Dir dir) {
		x.set(x*dir.x+y*dir.y);
		y.set(y*dir.x-x*dir.y);
	}

	Dir(X x, Y y) : x(x), y(y) {}
	Dir(const ericsson2017::protocol::Direction dir) : Dir(
		( dir == ericsson2017::protocol::Direction::UP ) ? -1 : ( dir == ericsson2017::protocol::Direction::DOWN ) ? 1 : 0,
		( dir == ericsson2017::protocol::Direction::LEFT ) ? -1 : ( dir == ericsson2017::protocol::Direction::RIGHT ) ? 1 : 0
	) {}
	Dir(const ericsson2017::protocol::Enemy::Direction::Reader dir) : Dir(
		( dir.getVertical() == ericsson2017::protocol::Direction::UP ) ? -1 : ( dir.getVertical() == ericsson2017::protocol::Direction::DOWN ) ? 1 : 0,
		( dir.getHorizontal() == ericsson2017::protocol::Direction::LEFT ) ? -1 : ( dir.getHorizontal() == ericsson2017::protocol::Direction::RIGHT ) ? 1 : 0
	) {}
	bool operator==(const Dir& other) const {
		return x==other.x && y==other.y;
	}
	bool operator!=(const Dir& other) const {
		return !(*this == other);
	}
	explicit operator ericsson2017::protocol::Direction() const {
		using namespace ericsson2017::protocol;
		if ( x>0 && y==0 ) {
			return Direction::DOWN;
		} else if ( x<0 && y==0 ) {
			return Direction::UP;
		} else if ( x==0 && y>0 ) {
			return Direction::RIGHT;
		} else if ( x==0 && y<0 ) {
			return Direction::LEFT;
		} else {
			throw logic_error("Failed to classify direction");
		}
	}
};

Dir randomDir(){
	switch (rand()%4) {
		case 0: return Dir(1, 0);
		case 1: return Dir(-1, 0);
		case 2: return Dir(0, 1);
		case 3: return Dir(0, -1);
	}
}

struct Pos {
	X x;
	Y y;

	void go(const Dir dir) {
		x.set(x+dir.x);
		y.set(y+dir.y);
	}

	Pos(X x, Y y) : x(x), y(y) {}
	Pos(const ericsson2017::protocol::Position::Reader pos) : Pos(pos.getX(), pos.getY()) {}

	operator string() const {
		return (string)"("+(string)x+(string)","+(string)")";
	}

	template <typename T>
	operator T() const {
		return x * X::MAX + y;
	}
	bool operator==(const Pos& other) const {
		return x==other.x && y==other.y;
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

template<typename E, typename Base>
bool tryPushGo(queue<E>& gray, Pos pos, ericsson2017::protocol::Direction dir, Base base) {
	try {
		gray.push(E(copy_call(pos, go(dir)), base));
		return true;
	} catch (domain_error err) {
		return false;
	}
}

struct State {
	Value<int> level;
	Value<int> tick;
	Cells cells;
	Enemies enemies;
	Unit unit;

	bool my(Cell* cell) const {
		return cell->owner == 0;
	}
	bool my(Pos pos) const {
		return my(cells[pos]);
	}
	bool my() const {
		return my(unit.pos);
	}

	static bool kills(Pos pos, size_t ticks, State state) {
		if (ticks==0) return false;
		{
			size_t enemies_size_before = state.enemies.size(); // Store size in variable to avoid infinite recursion
			for (size_t i=0; i<enemies_size_before; i++) {
				Enemy& enemy = *state.enemies[i];
				Pos next_pos = copy_call(enemy.pos, go(enemy.dir));
				if (state.my(next_pos)) { // Bouncing - more possible outcome
					bool moved = false;
					{
						Dir test_dir = enemy.dir;
						do {
							if (!state.my(copy_call(enemy.pos, go(test_dir)))) {
								try {
									if (!moved) {
										enemy.pos.go(test_dir);
										moved = true;
									} else { //moved
										state.enemies.push_back(new Enemy(copy_call(enemy, pos.go(test_dir))));
									}
								} catch (domain_error) {}
							}

							test_dir.turn(Dir(0, 1));
						} while (test_dir != enemy.dir);
					}
					if (!moved) {
						{
							Dir test_dir = enemy.dir;
							do {
								try {
									if (!moved) {
										enemy.pos.go(test_dir);
										moved = true;
									} else { //moved
										state.enemies.push_back(new Enemy(copy_call(enemy, pos.go(test_dir))));
									}
								} catch (domain_error) {}

								test_dir.turn(Dir(0, 1));
							} while (test_dir != enemy.dir);
						}
					}
				} else {
					try {
						enemy.pos.go(enemy.dir);
					} catch (domain_error) {}
				}
			}
		}
		for (auto enemy : state.enemies) {
			if (enemy->pos==pos) return true;
		}
		return kills(pos, ticks-1, state);
	}

	bool kills(Pos pos, size_t ticks) const {
		return kills(pos, ticks, *this);
	}

	State(const ericsson2017::protocol::Response::Reader response) : level(response.getInfo().getLevel()), tick(response.getInfo().getTick()), cells(response.getCells()), enemies(response.getEnemies()), unit(response.getUnits()[0]) {}

	struct BacktrackInfo { // It's tdk a function
		optional<Dir> dir;
		size_t length = 0;

		BacktrackInfo next(Dir dir) {
			BacktrackInfo newinfo = BacktrackInfo(*this);
			newinfo.length++; newinfo.dir = dir;
			return newinfo;
		}
		BacktrackInfo(Dir dir, size_t length) : dir(dir), length(length) {}
	};

	optional<BacktrackInfo> backtrack(Dir dir, Pos pos) const {
		if (my(pos)) {
			ericsson2017::protocol::log("Backtrack succeeded");
			return BacktrackInfo(randomDir(), 1);
		}

		{ // Try forward
			optional<BacktrackInfo> info = backtrack(dir, copy_call(pos, go(dir)));
			if (info && !kills(pos, info->length)) return info->next(dir);
		}

		{ // Try right
			dir.turn(Dir(0, 1));
			optional<BacktrackInfo> info = backtrack(dir, copy_call(pos, go(dir)));
			if (info && !kills(pos, info->length)) return info->next(dir);
		}

		{ // Try left
			dir.turn(Dir(-1, 0));
			optional<BacktrackInfo> info = backtrack(dir, copy_call(pos, go(dir)));
			if (info && !kills(pos, info->length)) return info->next(dir);
		}

		// else
		return nullopt;
	}

	ericsson2017::protocol::Direction directToNextAttackable() const {
		using namespace ericsson2017::protocol;
		struct E {
			Pos pos;
			Direction startdir;
			operator Pos(){
				return pos;
			}
			E(Pos pos, E e) : pos(pos), startdir(e.startdir) {}
			E(Pos pos, Direction startdir) : pos(pos), startdir(startdir) {}
		};
		queue<E> gray;

		 tryPushGo(gray, unit.pos, Direction::LEFT	,  Direction::LEFT	);
		 tryPushGo(gray, unit.pos, Direction::RIGHT	,  Direction::RIGHT	);
		 tryPushGo(gray, unit.pos, Direction::UP	,  Direction::UP	);
		 tryPushGo(gray, unit.pos, Direction::DOWN	,  Direction::DOWN	);
		while (!gray.empty()) {
			E e = gray.front(); gray.pop();
			if (my(e)) return e.startdir;
			else {
				tryPushGo(gray, e.pos, Direction::LEFT	, e);
				tryPushGo(gray, e.pos, Direction::RIGHT	, e);
				tryPushGo(gray, e.pos, Direction::UP	, e);
				tryPushGo(gray, e.pos, Direction::DOWN	, e);
			}
		}
		/* else */ return (Direction)randomDir();
	}

	ericsson2017::protocol::Direction getNextDirection() const {
		using namespace ericsson2017::protocol;
		switch (my()) {
			case true:
				{
					optional<BacktrackInfo> info = backtrack(unit.dir, unit.pos);
					if (info && info->dir) return info->dir->operator ericsson2017::protocol::Direction();
				}
				return (Direction)unit.dir;

			case false:
				return directToNextAttackable();
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
