/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "interface.h"

#include "Command.capnp.h"
#include "Response.capnp.h"

#include <capnp/message.h>
#include <capnp/serialize.h>

#include <string>
#include <iostream> // For usage message

using namespace ericsson2017::protocol;

char* hash;
char* username;
const int receive_fd = 3;
const int send_fd = 4;


void login() {
	::capnp::MallocMessageBuilder message;
	Command::Builder command = message.initRoot<Command>();

	Command::Login::Builder login = command.initLogin();
	login.setTeam(username);
	login.setHash(hash);

	log("Sending login information...");
	::capnp::writeMessageToFd(send_fd, message);
}

void response(void (*response_handler)(const Response::Reader&), void (*status_handler)(const ::capnp::Text::Reader&)) {
	::capnp::StreamFdMessageReader msg(receive_fd);
	Response::Reader resp = msg.getRoot<Response>();
	
	if (status_handler != nullptr) status_handler(resp.getStatus());
	if (response_handler != nullptr) response_handler(resp);
}

void request(void (*move_handler)(Move::Builder&)) {
	::capnp::MallocMessageBuilder message;
	Command::Builder command = message.initRoot<Command>();

	if (move_handler != nullptr) {
		auto moves = command.initMoves(1);
		auto move = moves[0];
		move_handler(move);
	}
}


int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr<<"Usage: "<<argv[0]<<" username hash"<<std::endl;
		return 1;
	} else {
		username = argv[1];
		log((std::string)"Using username "+(std::string)username);

		hash = argv[2];
		log((std::string)"Using hash "+(std::string)hash);
	}

	login();
	while (true) {
		response(draw_response, write_status);
		request();
	}

	return 0;
}
