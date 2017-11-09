/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "main.h"
#include "interface.h"

#include <string>

using namespace ericsson2017::protocol;

int cnt;
int curr;

_SETUP {
	cnt=0;
	curr=0;
}

_MAIN_LOOP {
	if (cnt==curr*4){
		log("Square finished.");
		curr++; cnt=0;
	}

	move.setUnit(0);
	switch(cnt/curr) {
		case 0: move.setDirection(Direction::RIGHT);	break;
		case 1: move.setDirection(Direction::DOWN);	break;
		case 2: move.setDirection(Direction::LEFT);	break;
		case 3: move.setDirection(Direction::UP);	break;
	}

	cnt++;
}
