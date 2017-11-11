#include "main.h"
#include "interface.h"

#include <string>
#include <cstdlib>
#include <ncurses.h>

using namespace ericsson2017::protocol;

const size_t a = 19; // Square size
const size_t max_square_num = 80/(a+1);

enum Stage {
	initial,
	move_to_lastStatus,
	begin,	// Begin the square
	cont,	// Continue it
	end,	// Go back to my area
	prep,	// Prepare for next square: Move to starting position
	turn	// Make a U-turn
} stage;
struct SquareStatus {
	int32_t x, y;
	Direction direction; // Everybody moves while draws squares...
	size_t square_num; // How many squares did we do in a column?
	bool saved;
	size_t cnt; // Steps are taken in the stage
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

_MAIN_LOOP {
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
			move.setDirection(Direction::UP);
			stage = begin;
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

		case begin:
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
			}
			break;

		case cont:
			move.setDirection(status.direction);
			if ( ++status.cnt == a ) {
				status.cnt = 0;
				stage = end;
			}
			if ( response.getUnits()[0].getPosition().getX() == (status.direction==Direction::DOWN?79:0) ) {
				log("Error: We are near hitting a wall. Making an emergency (and economic) U-turn");
				status.square_num = 0;
				move.setDirection(Direction::RIGHT); status.cnt = 1;
				status.direction = opposite(status.direction);
				stage = begin;
				status.saved = false;
			}
			break;

		case end:
			move.setDirection(Direction::LEFT);
			if ( ++status.cnt == a ) {
				log("Square finished");
				status.cnt = 0;
				stage = prep;
			}

			if ( status.square_num+1 == max_square_num ) {
				log("max_square_num reached, making an enconomic U-turn");
				status.square_num = 0;
				move.setDirection(Direction::RIGHT); status.cnt = (-status.cnt) + 1 + 1;
				status.direction = opposite(status.direction);
				stage = begin;
				status.saved = false;
			}

			break;

		case prep:
			status.saved = false;
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
			}
			break;
	}
}
