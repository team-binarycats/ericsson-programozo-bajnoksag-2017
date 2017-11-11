#include "main.h"
#include "interface.h"

#include <ncurses.h>
#include <cstdlib>

using namespace ericsson2017::protocol;

_SETUP {
	initscr();
}

_MAIN_LOOP {
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

		default:
			move.setDirection(response.getUnits()[0].getDirection());
	}
}
