/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "Request.capnp.h"
#include "Response.capnp.h"
//#include "Bugfix.capnp.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <iostream>

using namespace ericsson2017::protocol::test;

char* hash;
char* username;

void sendRequest(Bugfix::Reader* bugfix, bool login = false) {
	::capnp::MallocMessageBuilder message;
	Request::Builder request = message.initRoot<Request>();
	
	if (login) {
		Request::Login::Builder login = request.initLogin();
		login.setTeam(username);
		login.setHash(hash);
	}

	if (bugfix != nullptr) {
		request.setBugfix(*bugfix);
	}

	::capnp::writeMessageToFd(1, message);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr<<"Usage: "<<argv[0]<<" username hash"<<std::endl;
		return 1;
	} else {
		username = argv[1];
		std::cerr<<"Using username "<<username<<std::endl;

		hash = argv[2];
		std::cerr<<"Using hash "<<hash<<std::endl;
	}

	sendRequest(nullptr, true);

	return 0;
}
