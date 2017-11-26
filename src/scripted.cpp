#include "main.h"

#include "interface.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace ericsson2017::protocol;

void trim(char * s) {
	char * p = s;
	int l = strlen(p);

	for (int i=0; i<l; i++)
		if ( p[i] == '#' ) {
			l = i; p[l]=0;
			break;
		}
	
	while(isspace(p[l - 1])) p[--l] = 0;
	while(* p && isspace(* p)) ++p, --l;
	
	memmove(s, p, l + 1);
}

_SETUP {}

FILE* cs = fdopen(5, "r");
char* cmd = NULL;
size_t len = 0;
ssize_t read;

_MAIN_LOOP {
	move.setUnit(0);
	
	while (true) {
		if (getline(&cmd, &len, cs)<2) throw std::length_error("Unable to read next command");
		trim(cmd);

		if ( cmd[0]==']' ) {
			log((std::string)"Comment: "+std::string(cmd));
			continue;
		}
		else if ( strcmp(cmd, "LEFT"	) == 0 ) move.setDirection(Direction::LEFT	);
		else if ( strcmp(cmd, "RIGHT"	) == 0 ) move.setDirection(Direction::RIGHT	);
		else if ( strcmp(cmd, "UP"	) == 0 ) move.setDirection(Direction::UP	);
		else if ( strcmp(cmd, "DOWN"	) == 0 ) move.setDirection(Direction::DOWN	);
		else throw std::domain_error((std::string)"Could not understand command \""+(std::string)cmd+(std::string)"\"");

		break;
	}
}
