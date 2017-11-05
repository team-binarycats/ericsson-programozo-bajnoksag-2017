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
#include <unistd.h>
#include <exception>
#include <cstring>

using namespace ericsson2017::protocol::test;

char* hash;
char* username;
const int receive_fd = 3;
const int send_fd = 3;


void log(std::string message) {
	std::cerr<<"* "<<message;
}

void write_bugfix(const Bugfix::Reader& bugfix) {
	std::cerr<<"> ";
	std::cout<<(int)bugfix.getBugs()<<" "<<(std::string)bugfix.getMessage()<<std::endl;
	if (!isatty(fileno(stdout))) std::cerr<<"#"<<(int)bugfix.getBugs()<<": \""<<(std::string)bugfix.getMessage()<<"\""<<std::endl;
}

void write_status(const ::capnp::Text::Reader& status) {
	std::cerr<<"] ";
	std::cerr<<(std::string)status<<std::endl;
}

void read_bugfix(Bugfix::Builder& bugfix) {
	uint8_t bugs;
	std::string message;

	std::cerr<<"< ";
	std::cin>>bugs;
	std::getline(std::cin, message);

	if (std::cin.eof()) {
		std::cerr<<"* "<<"End of input before got an end response"<<std::endl;
		throw std::runtime_error("End of input");
	}

	bugfix.setBugs(bugs);
	bugfix.setMessage(message);

	if (!isatty(fileno(stdin))) std::cerr<<"#"<<(int)bugs<<": \""<<(std::string)message<<"\""<<std::endl;
}


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

void request(void (*bugfix_builder)(Bugfix::Builder&), bool login = false) {
	::capnp::MallocMessageBuilder msg;
	Request::Builder req = msg.initRoot<Request>();

	if (login) {
		Request::Login::Builder lgn = req.initLogin();
		lgn.setTeam(username);
		lgn.setHash(hash);
	}

	Bugfix::Builder bgfx = req.initBugfix();
	bugfix_builder(bgfx);

	::capnp::writeMessageToFd(send_fd, msg);
}


bool response(void (*bugfix_handler)(const Bugfix::Reader&), void (*status_handler)(const ::capnp::Text::Reader&)) {
	::capnp::StreamFdMessageReader msg(receive_fd);

	Response::Reader resp = msg.getRoot<Response>();

	status_handler(resp.getStatus());

	if (resp.hasBugfix()) {
		bugfix_handler(resp.getBugfix());
	} else {
		log("No bugfix got!");
	}

	if (resp.isEnd()) {
		log((std::string)"Got end status: "+(resp.getEnd() ? (std::string)"true" : (std::string)"false"));
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
		log((std::string)"Using username "+(std::string)username);

		hash = argv[2];
		log((std::string)"Using hash "+(std::string)hash);
	}

	login();
	while (!response(&write_bugfix, &write_status)) {
		request(&read_bugfix);
	}

	return 0;
}
