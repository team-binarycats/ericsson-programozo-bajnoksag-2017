/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "interface.h"
#include "engine.h"

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

	::capnp::writeMessageToFd(send_fd, message);
}


int cnt=0;
int curr=1;
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
		response([](const Response::Reader& response){
			draw_response(response);
			save_response(response);
		}, write_status);

		if (cnt==curr*4){
			log("Square finished.");
			curr++; cnt=0;
		}
		request([](Move::Builder& move){
			move.setUnit(0);
			switch(cnt/curr) {
				case 0: move.setDirection(Direction::RIGHT);	break;
				case 1: move.setDirection(Direction::DOWN);	break;
				case 2: move.setDirection(Direction::LEFT);	break;
				case 3: move.setDirection(Direction::UP);	break;
			}
		});
		cnt++;
	}

	return 0;
}
