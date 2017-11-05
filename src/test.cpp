/** Tester for main.cpp (output side)
 * Dumps what flows through
 */

#include "Request.capnp.h"
#include "Response.capnp.h"
#include "Bugfix.capnp.h"

#include "interface.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <string>

using namespace ericsson2017::protocol::test;

const int receive_fd = 0;

void request(void (*bugfix_handler)(const Bugfix::Reader&)) {
	::capnp::StreamFdMessageReader msg(receive_fd);
	Request::Reader req = msg.getRoot<Request>();

	if (req.isLogin()) {
		Request::Login::Reader lgn = req.getLogin();
		log((::std::string)"Got login info: "+(::std::string)lgn.getTeam()+(::std::string)":"+(::std::string)lgn.getHash());
	}
	
	if (req.hasBugfix()) {
		bugfix_handler(req.getBugfix());
	} else {
		log("Got no bugfix");
	}
}

int main() {
	while (true) {
		request(&write_bugfix);
	}
}
