#include "main.h"
#include "interface.h"
#include <bits/stdc++.h>
using namespace ericsson2017::protocol;
using namespace std;

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
	throw domain_error("Bad direction");
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
	throw domain_error("Bad direction");
}

Direction prev (Direction direction) {
	return next(next(next(direction)));
}

string to_string(Direction direction) {
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

int extract_x(Direction direction) {
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

int extract_y(Direction direction) {
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
	if (vertical==true) {
		reset_vertical();
		next_split();
	} else {
		vertical=true;
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

bool freetil(const Response::Reader& response, vector<E> es, unsigned time, size_t x, size_t y) {
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

bool safe(const Response::Reader& response, size_t x, size_t y, size_t time) {
	vector<E> es;
	for (auto enemy : response.getEnemies()) {
		es.push_back(E(enemy));
	}
	return freetil(response, es, time, x, y);
}

bool safe(const Response::Reader& response, unsigned cnt, size_t x, size_t y, int v_x, int v_y, size_t time) {
	for (unsigned i=cnt; i>0; i--) {
		if ( !safe(response, x-v_x*(cnt-i+1), y-v_y*(cnt-i+1), time) ) return false;
	}
	return true;
}

bool safe(const Response::Reader& response, unsigned cnt, size_t x, size_t y, int v_x, int v_y) {
	return safe(response, cnt, x, y, v_x, v_y, cnt);
}

_MAIN_LOOP {
	if ( response.getCells()[response.getUnits()[0].getPosition().getX()][response.getUnits()[0].getPosition().getX()].getOwner() == 1 ) {
		reset_cnt();
	}

	if ( stage == doit ) {
		switch (direction) {
			case Direction::DOWN:	if (response.getUnits()[0].getPosition().getX()>=78)	next_col(); break;
			case Direction::UP:	if (response.getUnits()[0].getPosition().getX()<=1)	next_col(); break;
			case Direction::RIGHT:	if (response.getUnits()[0].getPosition().getY()>=98)	next_col(); break;
			case Direction::LEFT:	if (response.getUnits()[0].getPosition().getY()<=1)	next_col(); break;
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

				log((string)"Target: ("+to_string(target_x)+(string)","+to_string(target_y)+(string)") "+to_string(split)+(string)" "+to_string(col)+" "+(string)(vertical?"v":"h")+(string)" "+to_string(direction));

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
			if (safe(response, cnt,
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
