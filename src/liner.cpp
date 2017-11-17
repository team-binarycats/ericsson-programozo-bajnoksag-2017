#include "main.h"
#include <bits/stdc++.h>

using namespace ericsson2017::protocol;

Direction opposite (Direction direction) {
	switch (direction) {
		case Direction::UP:
			return Direction::DOWN;

		case Direction::LEFT:
			return Direction::RIGHT;

		case Direction::DOWN:
			return Direction::UP;

		case Direction::RIGHT:
			return Direction::LEFT;
	}
}

std::pair<size_t, size_t> go(Position::Reader pos, Direction dir) {
	switch (dir) {
		case Direction::UP:
			return {pos.getX()-1, pos.getY()};

		case Direction::LEFT:
			return {pos.getX(), pos.getY()-1};

		case Direction::DOWN:
			return {pos.getX()+1, pos.getY()};

		case Direction::RIGHT:
			return {pos.getX(), pos.getY()+1};
	}
}

enum Stage {
	attack,
	save,
	prep
} stage;

int cnt;

struct E {
	int x, y;
	short signed xd, yd;
	E(const Enemy::Reader& enemy){
		x = enemy.getPosition().getX();
		y = enemy.getPosition().getY();
		switch (enemy.getDirection().getVertical()) {
			case Direction::UP:
				xd = -1;
				break;

			case Direction::DOWN:
				xd = 1;
				break;

		}
		switch (enemy.getDirection().getHorizontal()) {
			case Direction::LEFT:
				yd = -1;
				break;

			case Direction::RIGHT:
				yd = 1;
				break;
		}
	}
};

_SETUP {
	stage = save;
}

_MAIN_LOOP {
	auto ismy = [&response](int x, int y){
		return response.getCells()[x][y].getOwner()==1;
	};

	std::function<bool(size_t, size_t, size_t, std::vector<E>)> recfreetil = [&](size_t x, size_t y, size_t time, std::vector<E> es){
		for (int i=0; i<es.size(); i++) {
			if ( es[i].x==x && es[i].y==y ) return false;
			es[i].x+=es[i].xd;
			es[i].y+=es[i].yd;
			if (ismy(es[i].x, es[i].y)) {
				E e2 = es[i];
				es[i].xd=-es[i].xd; es[i].x+=2*es[i].xd;
				es[i].yd=-es[i].yd; es[i].y+=2*es[i].yd;
				if (!ismy(e2.x-2*e2.xd, e2.y)) {
					e2.xd=-e2.xd; e2.x+=2*e2.xd;
					es.push_back(e2);
				} else if (!ismy(e2.x, e2.y-2*e2.yd)) {
					e2.yd=-e2.yd; e2.y+=2*e2.yd;
					es.push_back(e2);
				}
			}
		}
		if (time==0) return true;
		else return recfreetil(x, y,time-1, es);
	};

	auto freetil = [&](size_t x, size_t y, size_t time){
		std::vector<E> es;
		for (auto enemy : response.getEnemies()) es.push_back(E(enemy));
		return recfreetil(x, y,time, es);
	};

	auto isfreetil = [&](std::pair<size_t, size_t> pos, size_t time) {
		return freetil(pos.first, pos.second, time);
	};

	auto unit = response.getUnits()[0];
	move.setUnit(0);

	switch (stage) {
		case prep:
			if (ismy(unit.getPosition().getX(), unit.getPosition().getY())) {
				move.setDirection(unit.getDirection());
			} else {
				cnt = 0;
				move.setDirection(unit.getDirection());
				stage = attack;
			}
			break;	

		case attack:
			if (!isfreetil(go(unit.getPosition(), unit.getDirection()), cnt+1)) {
				move.setDirection(opposite(unit.getDirection()));
				stage = save;
			} else {
				move.setDirection(unit.getDirection());
				cnt++;
			}
			break;

		case save:
			if (ismy(unit.getPosition().getX(), unit.getPosition().getY())) {
				move.setDirection(opposite(unit.getDirection()));
				stage = prep;
			} else {
				move.setDirection(unit.getDirection());
			}
			break;
	}
}
