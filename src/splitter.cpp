#include "main.h"
#include "interface.h"
#include <bits/stdc++.h>
using namespace std;
using namespace ericsson2017::protocol;

const unsigned split = 3;
int current_col; // negative: vertical, positive: horizontal
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
			break;
	}
}
