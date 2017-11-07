#include "interface.h"

using namespace ericsson2017::protocol;

void ::ericsson2017::protocol::log(::std::string message) {
	::std::cerr<<"* "<<message<<::std::endl;
}

void ::ericsson2017::protocol::write_status(const ::capnp::Text::Reader& status) {
	::std::cerr<<"] ";
	::std::cerr<<(::std::string)status<<::std::endl;
}

void ::ericsson2017::protocol::write_response_human(const Response::Reader& response) {
	::std::ostream& os = std::cerr;
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
}
