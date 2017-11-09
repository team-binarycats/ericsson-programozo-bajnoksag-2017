#include "main.h"
#include "interface.h"

#include <string>

using namespace ericsson2017::protocol;

const size_t a = 19; // Square size
const size_t max_square_num = 80/(a+1);

enum Stage {
	initial,
	begin,	// Begin the square
	cont,	// Continue it
	end,	// Go back to my area
	prep,	// Prepare for next square: Move to starting position
	turn	// Make a U-turn
} stage;
size_t cnt; // Steps are taken in the stage
Direction direction; // Everybody moves while draws squares...
size_t square_num; // How many squares did we do in a column?

_SETUP {
	stage = initial;
	cnt = 0;
	direction = Direction::DOWN;
	square_num = 0;
}

_MAIN_LOOP {
	move.setUnit(0);
	switch (stage) {
		case initial:
			move.setDirection(Direction::UP);
			stage = begin;
			break;

		case begin:
			move.setDirection(Direction::RIGHT);
			if ( ++cnt == a ) {
				cnt = 0;
				stage = cont;
			}
			break;

		case cont:
			move.setDirection(direction);
			if ( ++cnt == a ) {
				cnt = 0;
				stage = end;
			}
			break;

		case end:
			move.setDirection(Direction::LEFT);
			if ( ++cnt == a ) {
				log("Square finished");
				cnt = 0;
				stage = prep;
			}
			break;

		case prep:
			if ( ++square_num == max_square_num ) {
				log("max_square_num reached, making a U-turn");
				square_num = 0;
				move.setDirection(Direction::RIGHT); cnt++;
				stage = turn;
			} else {
				move.setDirection(direction);
				stage = begin;
			}
			break;

		case turn:
			move.setDirection(Direction::RIGHT);
			if ( ++cnt == a ) {
				switch (direction) {
					case Direction::UP:	direction = Direction::DOWN;	break;
					case Direction::DOWN:	direction = Direction::UP;	break;
				}
				log("OK, turn succeeded, begining the next square");
				cnt = 0;
				move.setDirection(Direction::RIGHT); cnt++;
				stage = begin;
			}
			break;
	}
}
