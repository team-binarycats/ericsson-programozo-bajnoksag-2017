#include "main.h"
#include "interface.h"

#include <unistd.h>
#include <ncurses.h>
#include <cstdlib>

using namespace ericsson2017::protocol;

useconds_t usec;

_SETUP {
	usec = 1000000;
	initscr();
}

_MAIN_LOOP {
	usleep(usec);
	move.setUnit(0);
	switch(getch()) {
		case KEY_UP:
			move.setDirection(Direction::UP);
			break;

		case KEY_LEFT:
			move.setDirection(Direction::LEFT);
			break;

		case KEY_DOWN:
			move.setDirection(Direction::DOWN);
			break;

		case KEY_RIGHT:
			move.setDirection(Direction::RIGHT);
			break;

		case KEY_END:
			endiface();
			std::system("bin/sequential");
			initiface();
			break;

		case KEY_HOME:
			endiface();
			exit(0);
			break;

		case '+':
			usec = usec*4;
		case '-':
			usec = usec/2;
		default:
			auto dir = response.getUnits()[0].getDirection();
			auto x = response.getUnits()[0].getPosition().getX();
			auto y = response.getUnits()[0].getPosition().getY();
			if ( dir == Direction::UP && x == 0 ) {
				move.setDirection(Direction::DOWN);
			} else if ( dir == Direction::DOWN && x == 79 ) {
				move.setDirection(Direction::UP);
			} else if ( dir == Direction::LEFT && y == 0 ) {
				move.setDirection(Direction::RIGHT);
			} else if ( dir == Direction::RIGHT && y == 99 ) {
				move.setDirection(Direction::LEFT);
			} else {
				move.setDirection(response.getUnits()[0].getDirection());
			}
	}
}
