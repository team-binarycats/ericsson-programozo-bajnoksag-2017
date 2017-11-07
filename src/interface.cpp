#include "interface.h"

#include <iostream>
#include <unistd.h>
#include <exception>
#include <string>
#include <capnp/blob.h>
#include <sstream>

using namespace ericsson2017::protocol;

void ::ericsson2017::protocol::log(std::string message) {
	std::cerr<<"* "<<message<<std::endl;
}

void ::ericsson2017::protocol::write_status(const ::capnp::Text::Reader& status) {
	std::cerr<<"] ";
	std::cerr<<(std::string)status<<std::endl;
}

void ::ericsson2017::protocol::draw_response(const Response::Reader& response) {
	std::stringstream os;
	/* Screen layout:
	 *     2     101
	 *    +--------+
	 *  2 |        |
	 *    |        |
	 *    |        |
	 * 81 |        |
	 *    +--------+
	 * 83 | STATUS |
	 *    +--------+
	 */

	const char esc = '\033';
	const char* csi = "\033[";
	const char* sgr = csi;
	const char* sgr_end = "m";

	const char border_h = '-';
	const char border_v = '|';
	const char border_corner = '+';
	const char status_separator = '\t';
	const char cell_mark = '.';
	const char* enemy_ul = "↖";
	const char* enemy_ur = "↗";
	const char* enemy_dl = "↙";
	const char* enemy_dr = "↘";
	const char* unit_l = "⇦";
	const char* unit_r = "⇨";
	const char* unit_u = "⇧";
	const char* unit_d = "⇩";

	// Print border
	os<<csi<<"s";
	os<<csi<<"85;102H"<<csi<<"1J"; //ED
	for (int i=2; i<=83; i++) {
		os<<csi<<i<<";"<<"1H"<<border_v; // Left
		os<<csi<<i<<";"<<"102H"<<border_v; // Right
	}
	os<<csi<<"1;2H"; for(int i=2;i<=101;i++)os<<border_h; // Upper
	os<<csi<<"82;2H"; for(int i=2;i<=101;i++)os<<border_h; // Unter
	os<<csi<<"84;2H"; for(int i=2;i<=101;i++)os<<border_h; // Status
	os<<csi<<"1;1H"<<border_corner; // Upper left
	os<<csi<<"1;102H"<<border_corner; // Upper right
	os<<csi<<"82;1H"<<border_corner; // Unter left
	os<<csi<<"82;102H"<<border_corner; // Unter right
	os<<csi<<"84;1H"<<border_corner; // Status left
	os<<csi<<"84;102H"<<border_corner; // Status right
	os<<csi<<"u";

	// Info field
	Response::Info::Reader info = response.getInfo();
	os<<csi<<"s";
	os<<csi<<"83;3H";
	os<<"Owns: "<<info.getOwns()<<status_separator;
	os<<"Level: "<<info.getLevel()<<status_separator;
	os<<"Tick: "<<info.getTick()<<status_separator;
	os<<csi<<"u";

	// Cells
	for (int i=0; i<80; i++) {
		int I=i+2;
		for (int j=0; j<100; j++) {
			int J=j+2;
			Cell::Reader cell = response.getCells()[i][j];
			os<<csi<<"s";
			if (cell.getOwner()!=0)
				os<<sgr<<7<<sgr_end;
			if (cell.getAttack().isCan())
				if (cell.getAttack().getCan())
					os<<sgr<<32<<sgr_end;
				else // can't attack
					os<<sgr<<31<<sgr_end;
			if (cell.getAttack().isUnit())
				os<<sgr<<1<<sgr_end;
				// TODO indicate unit ID
			os<<csi<<I<<";"<<J<<"H"<<cell_mark;
			os<<sgr<<0<<sgr_end<<csi<<"u";
		}
	}

	// Enemies
	for (auto enemy : response.getEnemies()) {
		os<<csi<<"s";
		os<<csi<<enemy.getPosition().getX()+2<<";"<<enemy.getPosition().getY()+2<<"H";
		if (enemy.getDirection().getVertical() == Direction::UP)
			if (enemy.getDirection().getHorizontal() == Direction::LEFT) // Up-left
				os<<enemy_ul;
			else // Up-right
				os<<enemy_ur;
		else
			if (enemy.getDirection().getHorizontal() == Direction::LEFT) // Down-left
				os<<enemy_dl;
			else // Down-right
				os<<enemy_dr;
		os<<csi<<"u";
	}

	for (auto unit : response.getUnits()) {
		os<<csi<<"s";
		os<<csi<<unit.getPosition().getX()+2<<";"<<unit.getPosition().getY()+2<<"H";
		switch (unit.getHealth()) {
			case 3: os<<sgr<<37<<sgr_end; break;
			case 2: os<<sgr<<36<<sgr_end; break;
			case 1: os<<sgr<<35<<sgr_end; break;
			case 0: os<<sgr<<34<<sgr_end; break;
			default: throw std::runtime_error((std::string)"Invalid health: "+std::to_string(unit.getHealth()));
		}
		//TODO indicate unit killer level
		switch (unit.getDirection()) {
			case Direction::LEFT:	os<<unit_l; break;
			case Direction::RIGHT:	os<<unit_r; break;
			case Direction::UP:	os<<unit_u; break;
			case Direction::DOWN:	os<<unit_d; break;
		}
		os<<csi<<"u";
	}

	std::cerr<<os.str();
}
