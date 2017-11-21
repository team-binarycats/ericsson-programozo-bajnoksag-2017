#include "interface.h"
#include "engine.h"

#include <iostream>
#include <unistd.h>
#include <exception>
#include <string>
#include <capnp/blob.h>
#include <ncurses.h>
#include <signal.h>

using namespace ericsson2017::protocol;

enum : short {
	COLORPAIR_NORMAL,
	COLORPAIR_CELL_CANATTACK,
	COLORPAIR_CELL_CANTATTACK,
	COLORPAIR_UNIT_HEALTH_GOOD,
	COLORPAIR_UNIT_HEALTH_WEAK,
	COLORPAIR_UNIT_HEALTH_CRITICAL,
	COLORPAIR_UNIT_HEALTH_DEAD,
};

bool inited = false;

WINDOW *status_window;

void ::ericsson2017::protocol::initiface() {
	if (!inited) {
		initscr();
		start_color();
		curs_set(0); // invisible
		noecho();
		cbreak();
		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);
		init_pair(COLORPAIR_NORMAL, COLOR_BLACK, COLOR_WHITE);
		init_pair(COLORPAIR_CELL_CANATTACK, COLOR_BLACK, COLOR_GREEN);
		init_pair(COLORPAIR_CELL_CANTATTACK, COLOR_BLACK, COLOR_RED);
		init_pair(COLORPAIR_UNIT_HEALTH_GOOD, COLOR_BLACK, COLOR_WHITE);
		init_pair(COLORPAIR_UNIT_HEALTH_WEAK, COLOR_BLACK, COLOR_CYAN);
		init_pair(COLORPAIR_UNIT_HEALTH_CRITICAL, COLOR_BLACK, COLOR_MAGENTA);
		init_pair(COLORPAIR_UNIT_HEALTH_DEAD, COLOR_BLACK, COLOR_BLUE);
		{
			int row, col;
			getmaxyx(stdscr, row, col);
			if ( row<80 || col < 200 ) {
				endwin();
				std::cout<<"Your terminal ("<<row<<"x"<<col<<") is too small (at least 80x200 needed)";
				exit(1);
			}
			status_window = newwin(row-80, 200, 80, 0);
		}
		{
			auto signal_handler = [](int sig){
				endiface();
				SIG_DFL(sig);
			};
			signal(SIGABRT, signal_handler);
			signal(SIGFPE, signal_handler);
			signal(SIGILL, signal_handler);
			signal(SIGINT, signal_handler);
			signal(SIGSEGV, signal_handler);
			//signal(SIGTERM, signal_handler);
		}
		inited = true;
	}
}

void ::ericsson2017::protocol::endiface() {
	if (inited) {
		endwin();
		inited = false;
	}
}

void ::ericsson2017::protocol::log(std::string message) {
	std::cerr<<"* "<<message<<std::endl;
}

void ::ericsson2017::protocol::write_status(const ::capnp::Text::Reader& status) {
	std::cerr<<"] ";
	std::cerr<<(std::string)status<<std::endl;
}

void ::ericsson2017::protocol::draw_response(const Response::Reader& response) {
	if (!inited) initiface();

	typedef char schar[3];
	const char status_separator = '\t';
	const schar cell_mark = "[]";
	const schar enemy_ul =  "UL";
	const schar enemy_ur =  "UR";
	const schar enemy_dl =  "DL";
	const schar enemy_dr =  "DR";
	const schar unit_l =    "< ";
	const schar unit_r =    " >";
	const schar unit_u =    "/\\";
	const schar unit_d =    "\\/";


	auto draw_cell = [&](const Cell::Reader& cell, int x, int y) {
		if (cell.getOwner()==0) {
			attron(A_REVERSE);
		}
		if (cell.getAttack().isCan()) {
			if (cell.getAttack().getCan()) {
				attron(COLOR_PAIR(COLORPAIR_CELL_CANATTACK));
			} else { // can't attack
				attron(COLOR_PAIR(COLORPAIR_CELL_CANTATTACK));
			}
		}
		if (cell.getAttack().isUnit()) {
			attron(A_BOLD);
			// TODO indicate unit ID
		}
		mvaddstr(x, 2*y, cell_mark);

		standend();
		color_set(COLORPAIR_NORMAL, NULL);
	};

	// Info field
	if ( status_window != nullptr ) {
		Response::Info::Reader info = response.getInfo();
		werase(status_window);
		int w, h;
		getmaxyx(status_window, h, w);

		wmove(status_window, h/2, h>1 ? (h-1)/2 : 0); wattrset(status_window, A_BOLD);
		wprintw(status_window, "Owns: %d", info.getOwns()); waddch(status_window, status_separator);
		wprintw(status_window, "Level: %d", info.getLevel()); waddch(status_window, status_separator);
		wprintw(status_window, "Tick: %d", info.getTick()); waddch(status_window, status_separator);
		wprintw(status_window, "Unit0 position: (%d,%d)", response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY()); waddch(status_window, status_separator);
		{
			int mycells=0; for(int i=0;i<80*100;i++)if(response.getCells()[i%80][i/80].getOwner()==1)mycells++;
			wprintw(status_window, "Left: %.2f%%", 75.00-mycells/80); waddch(status_window, status_separator);
		}

		wmove(status_window, h-1, h>1 ? (h-1)/2-1 : 0); wattrset(status_window, A_NORMAL);
		wprintw(status_window, "Program: '%s' (compiled on %s %s)", __FILE__, __DATE__, __TIME__);
		wrefresh(status_window);
	}

	// Cells
	for (int i=0; i<80; i++) {
		for (int j=0; j<100; j++) {
			Cell::Reader cell = response.getCells()[i][j];
			/* {
				Cell::Reader ocell = response.getCells()[i][j];
				if (
					(cell.getOwner()==ocell.getOwner())
					&& (cell.getAttack().which()==ocell.getAttack().which())
					&& (	  cell.getAttack().isCan() ? (cell.getAttack().getCan()==ocell.getAttack().getCan())
						: cell.getAttack().isUnit() ? (cell.getAttack().isUnit()==ocell.getAttack().getUnit())
						: true
					)
				) continue;
			} */ // Caching isn't working properly yet :(
			draw_cell(cell, i, j);
		}
	}
	for (auto enemy : lastResponse.getEnemies()) {
		auto x = enemy.getPosition().getX();
		auto y = enemy.getPosition().getY();
		draw_cell(response.getCells()[x][y], x, y);
	}
	for (auto unit : lastResponse.getUnits()) {
		auto x = unit.getPosition().getX();
		auto y = unit.getPosition().getY();
		draw_cell(response.getCells()[x][y], x, y);
	}

	// Enemies
	for (auto enemy : response.getEnemies()) {
		auto x = enemy.getPosition().getX(), y = enemy.getPosition().getY();
		if (enemy.getDirection().getVertical() == Direction::UP)
			if (enemy.getDirection().getHorizontal() == Direction::LEFT) // Up-left
				mvaddstr(x, 2*y, enemy_ul);
			else // Up-right
				mvaddstr(x, 2*y, enemy_ur);
		else
			if (enemy.getDirection().getHorizontal() == Direction::LEFT) // Down-left
				mvaddstr(x, 2*y, enemy_dl);
			else // Down-right
				mvaddstr(x, 2*y, enemy_dr);
	}

	for (auto unit : response.getUnits()) {
		auto x = unit.getPosition().getX(), y = unit.getPosition().getY();
		switch (unit.getHealth()) {
			case 3: attron( COLOR_PAIR(COLORPAIR_UNIT_HEALTH_GOOD) ); break;
			case 2: attron( COLOR_PAIR(COLORPAIR_UNIT_HEALTH_WEAK) ); break;
			case 1: attron( COLOR_PAIR(COLORPAIR_UNIT_HEALTH_CRITICAL) ); break;
			case 0: attron( COLOR_PAIR(COLORPAIR_UNIT_HEALTH_DEAD) ); break;
			default: throw std::runtime_error((std::string)"Invalid health: "+std::to_string(unit.getHealth()));
		}
		//TODO indicate unit killer level
		switch (unit.getDirection()) {
			case Direction::LEFT:	mvaddstr(x, 2*y, unit_l); break;
			case Direction::RIGHT:	mvaddstr(x, 2*y, unit_r); break;
			case Direction::UP:	mvaddstr(x, 2*y, unit_u); break;
			case Direction::DOWN:	mvaddstr(x, 2*y, unit_d); break;
		}
		color_set(COLORPAIR_NORMAL, NULL);
	}

	refresh();
}
