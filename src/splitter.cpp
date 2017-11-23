#include "main.h"
#include "interface.h"
#include <bits/stdc++.h>
using namespace std;
using namespace ericsson2017::protocol;

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

Direction next (Direction direction) {
	switch (direction) {
		case Direction::UP:
			return Direction::RIGHT;

		case Direction::LEFT:
			return Direction::UP;

		case Direction::DOWN:
			return Direction::LEFT;

		case Direction::RIGHT:
			return Direction::DOWN;
	}
}

Direction prev (Direction direction) {
	return next(next(next(direction)));
}

enum {
	move_to,
	doit,
} stage;

unsigned checkpoint; // last safe x (or y) in current col
void reset_checkpoint(){
	checkpoint=0;
}

unsigned split;
void reset_split(){
	split=3;
}
void next_split(){
	split*=2;
	// no maximum here :)
}

bool vertical;
void reset_vertical(){
	vertical=false;
}
void next_side(){
	if (vertical==true) {
		reset_vertical();
		next_split();
	} else {
		vertical=true;
	}
}

unsigned col;
void reset_col(){
	col=0;
}
void next_col(){
	if (++col==split) {
		reset_col();
		next_side();
	}
	reset_checkpoint();
	stage=move_to;
}

unsigned cnt;
void reset_cnt(){
	cnt=0;
}

_SETUP {
	(void)level; // unused
	reset_cnt();
	stage = move_to;
	switch (reason) {
		case SetupReason::INIT:
		case SetupReason::LEVELUP:
		case SetupReason::LEVELDOWN:
			reset_split();
			reset_checkpoint();
			reset_vertical();
			reset_col();
			break;

		case SetupReason::DEATH:
		case SetupReason::HEAL:
			// Moves to latest checkpoint automatically
			break;
	}
}

bool safe(const Response::Reader& response, unsigned cnt, size_t x, size_t y);

_MAIN_LOOP {
	switch (stage) {
		case move_to:
			//TODO code here
			break;

		case doit:
			if (safe(response, cnt,
						response.getUnits()[0].getPosition().getX()+( vertical ? 1 : 0 ),
						response.getUnits()[0].getPosition().getY()+( vertical ? 0 : 1 )
			)) {
				move.setDirection(vertical ? Direction::DOWN : Direction::RIGHT);
				cnt++;
			} else {
				move.setDirection(vertical ? Direction::UP : Direction::LEFT);
				cnt--;
			}

			if (vertical) {
				if (response.getUnits()[0].getPosition().getX()>=78) {
					next_col();
				}
			} else { // horizontal
				if (response.getUnits()[0].getPosition().getY()>=98) {
					next_col();
				}
			}
			break;
	}
}
