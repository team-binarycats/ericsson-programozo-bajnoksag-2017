#pragma once

#include "Command.capnp.h"
#include "Response.capnp.h"

#include <iostream>

extern char* username;
extern char* hash;

enum class SetupReason {
	INIT,
	LEVELUP,
	LEVELDOWN
};

//int main(char* argc, char* argv); // Defined...

#define _SETUP void setup(int32_t level, SetupReason reason)
_SETUP;

#define _MAIN_LOOP void loop(const ::ericsson2017::protocol::Response::Reader& response, ::ericsson2017::protocol::Move::Builder& move)
_MAIN_LOOP;

//void success();
