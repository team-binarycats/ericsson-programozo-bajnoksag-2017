/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "Request.capnp.h"
#include "Response.capnp.h"
#include "Bugfix.capnp.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <iostream>
#include <string>

using namespace ericsson2017::protocol::test;

char* hash;
char* username;
const int receive_fd = 1;
const int send_fd = 1;

void login() {
	::capnp::MallocMessageBuilder message;
	Request::Builder request = message.initRoot<Request>();

	Request::Login::Builder login = request.initLogin();
	login.setTeam(username);
	login.setHash(hash);

	::capnp::writeMessageToFd(send_fd, message);
}

void sendEmptyRequest() {
	::capnp::MallocMessageBuilder message;
	message.initRoot<Request>();
	
	::capnp::writeMessageToFd(send_fd, message);
}

void request(uint8_t bugs, ::capnp::Text::Reader message, bool login = false) {
	::capnp::MallocMessageBuilder msg;
	Request::Builder req = msg.initRoot<Request>();

	if (login) {
		Request::Login::Builder lgn = req.initLogin();
		lgn.setTeam(username);
		lgn.setHash(hash);
	}

	Bugfix::Builder bgfx = req.initBugfix();
	bgfx.setBugs(bugs);
	bgfx.setMessage(message);

	::capnp::writeMessageToFd(send_fd, msg);
}


bool response() {
	::capnp::StreamFdMessageReader msg(receive_fd);

	Response::Reader resp = msg.getRoot<Response>();

	std::cerr<<"Status: "<<(std::string)resp.getStatus()<<std::endl;

	if (resp.hasBugfix()) {
		Bugfix::Reader bgfx = resp.getBugfix();
		std::cout<<bgfx.getBugs()<<" "<<(std::string)bgfx.getMessage()<<std::endl;
	}

	if (resp.isEnd()) {
		std::cerr<<"Got end status: "<<(resp.getEnd() ? "true" : "false")<<std::endl;
		return resp.getEnd();
	}

	return false;
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

	login();
	while (!response()) {
		uint8_t bugs;
		std::string message;

		std::cin>>bugs;
		std::getline(std::cin, message);
		if (std::cin.eof()) {
			std::cerr<<"End of input before got an end response"<<std::endl;
			return 1;
		}

		request(bugs, message);
	}

	return 0;
}
