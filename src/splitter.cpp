#include "main.h"
#include "interface.h"
#include <bits/stdc++.h>
using namespace std;
using namespace ericsson2017::protocol;

const unsigned split = 3;
unsigned current_col; // even: horizontal, odd: vertical
unsigned checkpoint; // last safe x (or y) in current col
unsigned cnt;
enum {
	move_to,
	doit,
} stage;

_SETUP {
	(void)level; // unused
	cnt = 0;
	stage = move_to;
	switch (reason) {
		case SetupReason::INIT:
		case SetupReason::LEVELUP:
		case SetupReason::LEVELDOWN:
			current = 0;
			checkpoint = 0;
			break;

		case SetupReason::DEATH:
		case SetupReason::HEAL:
			// Moves to latest checkpoint automatically
			break;
	}
}

_MAIN_LOOP {
	switch (stage) {
		case move_to:
			//TODO code here
			break;

		case doit:
			if (safe(response,
						response.getUnits()[0].getPosition().getX()+(  (current_col%2) ? +1 : 0 ),
						response.getUnits()[0].getPosition().getY()+( !(current_col%2) ? +1 : 0 )
			)) {
				move.setDirection(current_col%2 ? Direction::DOWN : Direction::RIGHT);
			} else {
				move.setDirection(current_col%2 ? Direction::UP : Direction::LEFT);
			}
			break;
	}
}
