/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "interface.h"

#include <string>

using namespace ericsson2017::protocol::test;

char* hash;
char* username;
const int receive_fd = 3;
const int send_fd = 4;


void login() {
	::capnp::MallocMessageBuilder message;

	login.setTeam(username);
	login.setHash(hash);

	log("Sending login information...");
	::capnp::writeMessageToFd(send_fd, message);
}

void response() {
	::capnp::StreamFdMessageReader msg(receive_fd);
}


int main(int argc, char* argv[]) {
	if (argc != 3) {
		::std::cerr<<"Usage: "<<argv[0]<<" username hash"<<::std::endl;
		return 1;
	} else {
		username = argv[1];
		log((::std::string)"Using username "+(::std::string)username);

		hash = argv[2];
		log((::std::string)"Using hash "+(::std::string)hash);
	}

	login();

	return 0;
}
