#include "main.h"
#include "interface.h"

#include <string>
#include <cstdlib>
#include <ncurses.h>
#include <functional>
#include <vector>
#include <utility>

using namespace ericsson2017::protocol;

const size_t a = 19; // Square size
const size_t max_square_num = 80/(a+1);

enum Stage {
	initial,
	move_to_lastStatus,
	begin,	// Begin the square
	cont,	// Longest side
	end,	// Go back to my area
	prep,	// Prepare for next square: Move to starting position
	turn,	// Make a U-turn
	stale_begin,
	stale_cont,
	stale_end,
} stage;
struct SquareStatus {
	int32_t x, y;
	Direction direction; // Everybody moves while draws squares...
	size_t square_num; // How many squares did we do in a column?
	bool saved;
	bool square_checked; // It is checked whether safe to make this square
	size_t cnt; // Steps are taken in the stage
	std::vector<std::pair<size_t, size_t>> affected_cells;
};
SquareStatus status, lastStatus;

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

_SETUP {
	status.cnt = 0;
	status.direction = Direction::DOWN;
	status.square_num = 0;
	status.saved = false;
	status.square_checked = false;
	status.affected_cells.clear();

	if ( reason == SetupReason::INIT ) {
		stage = initial;
	} else if ( reason == SetupReason::DEATH ) {
		log((std::string)"Restoring the last square at "+std::to_string(lastStatus.x)+(std::string)";"+std::to_string(lastStatus.y));
		status = lastStatus;
		stage = move_to_lastStatus;
	} else {
		stage = begin;
	}
}

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

	auto squarefree = [&](size_t x, size_t y, int shift=0){
		for (int i=0; i<a; i++) {
			if (!freetil(x, y, 1*a-i)) return false;
			y--;
		}
		return true;
	};

	switch (getch()) {
		case KEY_HOME:
			exit(0);
			break;

		case KEY_END:
			std::system("bin/keyboard");
			break;
	}
	move.setUnit(0);
	switch (stage) {
		case initial:
			{
				auto x = response.getUnits()[0].getPosition().getX();
				auto y = response.getUnits()[0].getPosition().getY();
				if ( x==0 ) {
					move.setDirection(Direction::RIGHT); status.cnt = y+1;
					stage = begin;
				} else if ( x>0 ) {
					move.setDirection(Direction::UP);
				} else {
					lastStatus = status;
					stage = move_to_lastStatus;
				}
			}
			break;

		case move_to_lastStatus:
			{
				int v_diff = lastStatus.x - response.getUnits()[0].getPosition().getX();
				int h_diff = lastStatus.y - response.getUnits()[0].getPosition().getY();
				if ( std::abs(v_diff) > std::abs(h_diff) ) {
					if ( v_diff > 0 ) {
						move.setDirection(Direction::DOWN);
					} else { // v_diff < 0
						move.setDirection(Direction::UP);
					}
				} else { // h_diff > v_diff
					if ( h_diff > 0 ) {
						move.setDirection(Direction::RIGHT);
					} else {
						move.setDirection(Direction::LEFT);
					}
				}
				if ( std::abs(v_diff)+std::abs(h_diff) == 1 ) {
					stage = begin;
					status = lastStatus;
				}
			}
			break;

		case stale_begin:
			move.setDirection(Direction::RIGHT);
			stage = begin;
			break;

		case begin:
			if (!freetil(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()+1, 2*a-status.cnt)) {
				log("Possible collision detected. Stalling for safety.");
				move.setDirection(Direction::LEFT);
				stage = stale_begin;
				break;
			}
			if ( !status.square_checked && !ismy(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()) ) {
				status.square_checked = true;
			}

			if ( !status.saved ) {
				status.x = response.getUnits()[0].getPosition().getX();
				status.y = response.getUnits()[0].getPosition().getY();
				lastStatus = status; log("Checkpoint saved");
				status.saved = true;
			}
			move.setDirection(Direction::RIGHT);
			if ( ++status.cnt == a ) {
				status.cnt = 0;
				stage = cont;
				status.affected_cells.clear();
			}
			break;

		case stale_cont:
			move.setDirection(status.direction);
			stage = cont;
			break;

		case cont:
			if (!freetil(response.getUnits()[0].getPosition().getX()+(status.direction==Direction::DOWN?1:-1), response.getUnits()[0].getPosition().getY(), a)) {
				log("Possible collision detected. Stalling for safety.");
				move.setDirection(opposite(status.direction));
				stage = stale_cont;
				break;
			}

			move.setDirection(status.direction);
			for (auto cell : status.affected_cells) {
				if (!freetil(cell.first, cell.second, a+1)) {
					log("Ouch! They try to hit our tail!");
					move.setDirection(Direction::LEFT);
					status.cnt = 1;
					stage = end;
					break;
				}
			}

			if ( response.getUnits()[0].getPosition().getX() == (status.direction==Direction::DOWN?79:0) ) {
				log("Error: We are near hitting a wall. Making an emergency (and economic) U-turn");
				status.square_num = 0;
				move.setDirection(Direction::RIGHT); status.cnt = 1;
				status.direction = opposite(status.direction);
				stage = begin;
				status.saved = false;
			}

			status.affected_cells.push_back(std::make_pair(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()));
			break;

		case stale_end:
			move.setDirection(Direction::LEFT);
			stage = end;
			break;

		case end:
			if (!freetil(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()-1, a-status.cnt)) {
				log("Possible collision detected. Stalling for safety.");
				move.setDirection(Direction::RIGHT);
				stage = stale_end;
				break;
			}

			move.setDirection(Direction::LEFT);
			if ( ++status.cnt == a ) {
				log("Square finished");
				status.cnt = 0;
				stage = prep;
			}

			if ( status.square_num+1 == max_square_num ) {
				if (!freetil(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()+1, 3*a)) {
					log("Possible collision detected. not making economic U-turn");
				} else {
					log("max_square_num reached, making an enconomic U-turn");
					status.square_num = 0;
					move.setDirection(Direction::RIGHT); status.cnt = (-status.cnt) + 1 + 1;
					status.direction = opposite(status.direction);
					stage = begin;
					status.saved = false;
				}
			}

			break;

		case prep:
			status.saved = false;
			status.square_checked = false;
			if ( ++status.square_num == max_square_num ) {
				log("max_square_num reached, making a U-turn");
				status.square_num = 0;
				move.setDirection(Direction::RIGHT); status.cnt++;
				stage = turn;
			} else {
				move.setDirection(status.direction);
				stage = begin;
			}
			break;

		case turn:
			move.setDirection(Direction::RIGHT);
			if ( ++status.cnt == a ) {
				status.direction = opposite(status.direction);
				log("OK, turn succeeded, begining the next square");
				status.cnt = 0;
				move.setDirection(Direction::RIGHT); status.cnt++;
				stage = begin;
				if (!freetil(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()+1, 3*a)) {
					log("Possible collision detected. Stalling for safety before next square.");
					move.setDirection(Direction::LEFT);
					stage = stale_begin;
					break;
				}
			}
			break;
	}
}
