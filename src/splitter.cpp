#include "main.h"
#include "interface.h"
#include <bits/stdc++.h>
using namespace ericsson2017::protocol;
using namespace std;

Direction opposite (const Direction& direction) {
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
	throw domain_error("Bad direction");
}

Direction next (const Direction& direction) {
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
	throw domain_error("Bad direction");
}

Direction prev (const Direction& direction) {
	return next(next(next(direction)));
}

string to_string(const Direction& direction) {
	switch (direction) {
		case Direction::UP:
			return "UP";

		case Direction::LEFT:
			return "LEFT";

		case Direction::DOWN:
			return "DOWN";

		case Direction::RIGHT:
			return "RIGHT";

		default:
			return "???";
	}
}

int extract_x(const Direction& direction) {
	switch (direction) {
		case Direction::UP:
			return -1;

		case Direction::LEFT:
			return 0;

		case Direction::DOWN:
			return +1;

		case Direction::RIGHT:
			return 0;
	}
	throw domain_error("Bad direction");
}

int extract_y(const Direction& direction) {
	return extract_x(next(direction));
}

enum {
	move_to,
	doit,
} stage;

unsigned checkpoint; // last safe x (or y) in current col
void reset_checkpoint(){
	checkpoint=0;
}
unsigned calc_cp(const size_t& x, const size_t& y, const Direction& direction) {
	switch (direction) {
		case Direction::DOWN:	return x-1;	break;
		case Direction::RIGHT:	return y-1;	break;
		case Direction::UP:	return 78-x;	break;
		case Direction::LEFT:	return 98-x;	break;
	}
}

Direction direction;
void reset_direction(){
	direction=Direction::DOWN; // dummy value
}

Direction side_direction;
void reset_side_direction(){
	side_direction=Direction::RIGHT; // dummy value
}

unsigned split;
void reset_split(){
	split=3;
}
void next_split(){
	log("Next split");
	split*=2;
	// no maximum here :)
}

bool vertical;
void reset_vertical(){
	vertical=true;
}
void next_side(){
	log("Next side");
	if (vertical==false) {
		reset_vertical();
		next_split();
	} else {
		vertical=false;
	}

	// You are not required to understand why this works
	swap(direction, side_direction); direction=opposite(direction); side_direction=opposite(side_direction);
}

unsigned col;
void reset_col(){
	col=1;
}
void next_col(){
	log("Next column");
	if (++col==split) {
		reset_col();
		next_side();
	}
	reset_checkpoint();
	direction=opposite(direction);
	stage=move_to;
}

unsigned cnt, maxcnt;
void reset_cnt(){
	maxcnt=cnt=0;
}

_SETUP {
	(void)level; // unused
	reset_cnt();
	stage = move_to;
	switch (reason) {
		case SetupReason::INIT:
		case SetupReason::LEVELUP:
		case SetupReason::LEVELDOWN:
			reset_direction();
			reset_side_direction();
			reset_split();
			reset_checkpoint();
			reset_vertical();
			reset_col();
			log("Reset completed");
			break;

		case SetupReason::DEATH:
		case SetupReason::HEAL:
			// Moves to latest checkpoint automatically
			break;
	}
}

struct E {
	size_t x, y;
	short signed xd, yd;
	E(const Enemy::Reader& enemy) :
		x(enemy.getPosition().getX()),
		y(enemy.getPosition().getY()),
		xd(extract_x(enemy.getDirection().getVertical())),
		yd(extract_y(enemy.getDirection().getHorizontal()))
	{}
};

bool freetil(const Response::Reader& response, vector<E> es, const unsigned& time, const size_t& x, const size_t& y) {
	if ( time == 0 ) return true;
	
	for (size_t i = 0, es_size_before = es.size(); i < es_size_before; i++) {
		size_t next_x = es[i].x+es[i].xd;
		size_t next_y = es[i].y+es[i].yd;
		if ( response.getCells()[next_x][next_y].getOwner() == 1 ) { // My cell => do the bounce
			E e = es[i];
			bool moved = false;
			for (int d=0; d<4; d++) { // bitfield: ...<x_dir><y_dir>
				size_t next_x = e.x + 1-d/2*2;
				size_t next_y = e.y + 1-d%2*2;
				if ( response.getCells()[next_x][next_y].getOwner() == 1 ) continue;
				else { // free route
					if (moved) {
						E e2 = e;
						e2.x = next_x;
						e2.y = next_y;
						e2.xd = 1-d/2*2;
						e2.yd = 1-d%2*2;
						es.push_back(e2);
					} else { // !moved
						E& e2 = es[i];
						e2.x = next_x;
						e2.y = next_y;
						e2.xd = 1-d/2*2;
						e2.yd = 1-d%2*2;
						moved = true;
					}
				}
			}
		} else { // straight line
			es[i].x = next_x;
			es[i].y = next_y;
		}
	}

	for (auto e : es) {
		if ( e.x==x && e.y==y ) return false;
	}

	return freetil(response, es, time-1, x, y);
}

bool safe(const Response::Reader& response, const size_t& x, const size_t& y, const size_t& time) {
	vector<E> es;
	for (auto enemy : response.getEnemies()) {
		es.push_back(E(enemy));
	}
	return freetil(response, es, time, x, y);
}

bool safe(const Response::Reader& response, const unsigned& before, const unsigned& after, const size_t& x, const size_t& y, const int& v_x, const int& v_y) {
	for (unsigned i=-before; i<=after; i++) {
		if ( !safe(response, x+v_x*i, y+v_y*i, before) ) return false;
	}
	return true;
}

_MAIN_LOOP {
	if ( response.getCells()[response.getUnits()[0].getPosition().getX()][response.getUnits()[0].getPosition().getX()].getOwner() == 1 ) {
		reset_cnt();
	}

	if ( stage == doit ) {
		if (cnt>maxcnt) maxcnt=cnt;
		switch (direction) {
			case Direction::DOWN:	if (response.getUnits()[0].getPosition().getX()>=78)	next_col(); break;
			case Direction::UP:	if (response.getUnits()[0].getPosition().getX()<=1)	next_col(); break;
			case Direction::RIGHT:	if (response.getUnits()[0].getPosition().getY()>=98)	next_col(); break;
			case Direction::LEFT:	if (response.getUnits()[0].getPosition().getY()<=1)	next_col(); break;
		}
	}

	if ( (stage == doit) && (response.getCells()[response.getUnits()[0].getPosition().getX()][response.getUnits()[0].getPosition().getX()].getOwner() == 1) ) {
		auto cp = calc_cp(response.getUnits()[0].getPosition().getX(), response.getUnits()[0].getPosition().getY(), direction);
		if  (cp>checkpoint) {
			checkpoint=cp;
			log("Checkpoint saved");
		}
	}

	switch (stage) {
		case move_to:
			{
				size_t target_x, target_y;
				if (vertical) {
					target_y = (size_t)100/split*col-1;
				} else {
					target_x = (size_t)80/split*col-1;
				}

				switch (direction) {
					case Direction::DOWN:	target_x = 1 + checkpoint;	break;
					case Direction::RIGHT:	target_y = 1 + checkpoint;	break;
					case Direction::UP:	target_x = 78 - checkpoint;	break;
					case Direction::LEFT:	target_y = 98 - checkpoint;	break;
				}

				signed v_diff = target_x - response.getUnits()[0].getPosition().getX();
				signed h_diff = target_y - response.getUnits()[0].getPosition().getY();

				log((string)"Target: ("+to_string(target_x)+(string)","+to_string(target_y)+(string)") s"+to_string(split)+(string)" c"+to_string(col)+" cp"+to_string(checkpoint)+(string)(vertical?" vertical ":" horizontal ")+to_string(direction));

				if ( abs(v_diff) > abs(h_diff) ) {
					if ( v_diff > 0 ) {
						move.setDirection(Direction::DOWN);
					} else { // v_diff < 0
						move.setDirection(Direction::UP);
					}
				} else { // h_diff > v_diff
					if ( h_diff > 0 ) {
						move.setDirection(Direction::RIGHT);
					} else { // h_diff < 0
						move.setDirection(Direction::LEFT);
					}
				}

				if ( abs(v_diff)+abs(h_diff) == 1 ) {
					stage = doit;
				}
			}
			break;

		case doit:
			if (safe(response, cnt, maxcnt-cnt,
						response.getUnits()[0].getPosition().getX()+( vertical ? 1 : 0 ),
						response.getUnits()[0].getPosition().getY()+( vertical ? 0 : 1 ),
			extract_x(direction), extract_y(direction))) {
				move.setDirection(direction);
				cnt++;
			} else {
				move.setDirection(opposite(direction));
				cnt--;
			}
			break;
	}
}
