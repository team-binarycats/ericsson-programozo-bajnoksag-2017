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

size_t size_at(const Direction& direction) {
	switch (direction) {
		case Direction::UP:
		case Direction::DOWN:
			return 80;

		case Direction::LEFT:
		case Direction::RIGHT:
			return 100;
	}
	throw domain_error("Bad direction");
}

size_t start_at(const Direction&) {
	return 2;
}

size_t end_at(const Direction& direction) {
	return size_at(direction)-start_at(direction)-1;
}

size_t start_x(const Direction& direction) {
	switch (direction) {
		case Direction::UP:
			return 77;

		case Direction::DOWN:
			return 2;
	}
	throw domain_error("Bad vertical direction");
}

size_t start_y(const Direction& direction) {
	switch (direction) {
		case Direction::LEFT:
			return 97;

		case Direction::RIGHT:
			return 2;
	}
	throw domain_error("Bad horizontal direction");
}

size_t end_x(const Direction& direction) {
	switch (direction) {
		case Direction::UP:
			return 2;

		case Direction::DOWN:
			return 77;
	}
	throw domain_error("Bad vertical direction");
}

size_t end_y(const Direction& direction) {
	switch (direction) {
		case Direction::LEFT:
			return 2;

		case Direction::RIGHT:
			return 97;
	}
	throw domain_error("Bad horizontal direction");
}

bool owns(const Response::Reader& response, const size_t& x, const size_t& y) {
	return response.getCells()[x][y].getOwner() == 1 && response.getCells()[x][y].getAttack().isCan();
}

enum {
	move_to,
	doit,
} stage;

unsigned checkpoint; // last safe x (or y) in current col
void reset_checkpoint(){
	checkpoint=0;
	log("Checkpoint reseted");
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
	split=4;
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
	swap(direction, side_direction); side_direction=opposite(side_direction);
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

unsigned calc_cp(const size_t& x, const size_t& y, const Direction& direction) {
	log(to_string(x)+(string)" ccp  "+to_string(y));
	switch (direction) {
		case Direction::DOWN:
		case Direction::UP:
			return extract_x(direction) * ( x-start_x(direction) ) + 1;

		case Direction::RIGHT:
		case Direction::LEFT:
			return extract_y(direction) * ( y-start_y(direction) ) + 1;
	}
}
size_t calc_side_offset(){
	return size_at(side_direction)/split*col-1;
}
size_t calc_cp_x(){
	if (vertical) {
		return start_x(direction) + (signed) extract_x(direction) * ((signed)checkpoint-1);
	} else {
		return extract_x(side_direction)>0 ? calc_side_offset() : size_at(side_direction)-calc_side_offset()-1;
	}
}
size_t calc_cp_y(){
	if (!vertical) {
		return start_y(direction) + (signed) extract_y(direction) * ((signed)checkpoint-1);
	} else {
		return extract_y(side_direction)>0 ? calc_side_offset() : size_at(side_direction)-calc_side_offset()-1;
	}
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
		if ( owns(response, next_x, next_y) ) { // My cell => do the bounce
			E e = es[i];
			bool moved = false;
			for (int d=0; d<4; d++) { // bitfield: ...<x_dir><y_dir>
				size_t next_x = e.x + 1-d/2*2;
				size_t next_y = e.y + 1-d%2*2;
				if ( owns(response, next_x, next_y) ) continue;
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
	log((string)"Checking ("+to_string(x)+(string)","+to_string(y)+(string)") for safety");
	vector<E> es;
	for (auto enemy : response.getEnemies()) {
		es.push_back(E(enemy));
	}
	return freetil(response, es, time, x, y);
}

bool safe(const Response::Reader& response, const unsigned& before, const unsigned& after, const size_t& x, const size_t& y, const int& v_x, const int& v_y, const size_t& time) {
	for (int i=-(int)before; i<=+(int)after; i++) {
		if ( !safe(response, x+v_x*i, y+v_y*i, time) ) return false;
	}
	return true;
}

bool allmy(const Response::Reader& response) { // Checks wether I own all cells in the specified row (or column)
	for (
		size_t x=calc_cp_x(), y=calc_cp_y();
		vertical ? x != end_x(direction)+extract_x(direction) : y != end_y(direction)+extract_y(direction);
		x+=extract_x(direction), y+=extract_y(direction)
	) {
		if ( !owns(response, x, y) ) {
			return false;
		}
	}
	return true;
}

void fix_cp(const Response::Reader& response) {
	for (
		size_t x=calc_cp_x(), y=calc_cp_y();
		vertical ? x != end_x(direction)+extract_x(direction) : y != end_y(direction)+extract_y(direction);
		x+=extract_x(direction), y+=extract_y(direction)
	) {
		if ( owns(response, x, y) ) {
			checkpoint = calc_cp(x, y, direction);
		} else break;
	}
}

_MAIN_LOOP {
	Unit::Reader unit = response.getUnits()[0];
	Cell::Reader cell = response.getCells()[unit.getPosition().getX()][unit.getPosition().getY()];

	if ( cell.getOwner() == 1 ) {
		reset_cnt();
	}

	if ( stage == doit ) {
		switch (direction) {
			case Direction::DOWN:	if (unit.getPosition().getX()>=78)	next_col(); break;
			case Direction::UP:	if (unit.getPosition().getX()<=1)	next_col(); break;
			case Direction::RIGHT:	if (unit.getPosition().getY()>=98)	next_col(); break;
			case Direction::LEFT:	if (unit.getPosition().getY()<=1)	next_col(); break;
		}
	}

	if ( (stage == doit) && (cell.getOwner() == 1) ) {
		auto cp = calc_cp(unit.getPosition().getX(), unit.getPosition().getY(), direction);
		if  (cp>checkpoint) {
			checkpoint=cp;
			log((string)"Checkpoint saved: "+to_string(checkpoint));
		}
	}

	while ( allmy(response) ) {
		log("I own everything here, economic row skip");
		next_col();
		fix_cp(response);
	}

	if ( stage == doit ) {
		if (cnt>maxcnt) maxcnt=cnt;
	}

	switch (stage) {
		case move_to:
			{
				size_t target_x = calc_cp_x();
				size_t target_y = calc_cp_y();

				signed v_diff = target_x - unit.getPosition().getX();
				signed h_diff = target_y - unit.getPosition().getY();

				log((string)"Target: ("+to_string(target_x)+(string)","+to_string(target_y)+(string)") s"+to_string(split)+(string)" c"+to_string(col)+" cp"+to_string(checkpoint)+(string)(vertical?" vertical ":" horizontal ")+to_string(direction)+(string)" ("+to_string(side_direction)+(string)")");

				if ( (abs(v_diff)>0) && (unit.getPosition().getY()<2 || unit.getPosition().getY()>97) ) {
					if ( v_diff > 0 ) {
						move.setDirection(Direction::DOWN);
					} else { // v_diff < 0
						move.setDirection(Direction::UP);
					}
				} else if ( (abs(h_diff)>0) && (unit.getPosition().getX()<2 || unit.getPosition().getX()>77) ) {
					if ( h_diff > 0 ) {
						move.setDirection(Direction::RIGHT);
					} else { // h_diff < 0
						move.setDirection(Direction::LEFT);
					}
				} else { // Dumb method
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
				}

				if ( abs(v_diff)+abs(h_diff) == 1 ) {
					log("Last move");
					stage = doit;
				}
			}
			break;

		case doit:
			log((string)"cnt="+to_string(cnt)+(string)"\tmaxcnt="+to_string(maxcnt));
			if ( maxcnt==cnt
					&& ( ( safe(response, cnt, max(maxcnt-cnt, (unsigned)1),
						unit.getPosition().getX(),
						unit.getPosition().getY(),
						extract_x(direction),
						extract_y(direction),
						cnt+2*(maxcnt-cnt)+2)
					) || safe(response, cnt, size_at(direction)-calc_cp(unit.getPosition().getX(), unit.getPosition().getY(), direction)-5,
						unit.getPosition().getX(),
						unit.getPosition().getY(),
						extract_x(direction),
						extract_y(direction),
						size_at(direction)-4-cnt)
			) ) {
				move.setDirection(direction);
				cnt++;
			} else {
				log("Ouch, unsafe!");
				move.setDirection(opposite(direction));
				cnt--;
			}
			break;
	}
}
