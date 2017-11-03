/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include <iostream>

using namespace ericsson2017::protocol::test;

char* server;
char* hash;
char* username;
int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr<<"Usage: "<<argv[0]<<" server username hash"<<std::endl;
		return 1;
	} else {
		server = argv[1];
		std::cerr<<"Using server "<<server<<std::endl;

		username = argv[2];
		std::cerr<<"Using username "<<username<<std::endl;

		hash = argv[3];
		std::cerr<<"Using hash "<<hash<<std::endl;
	}

	return 0;
}
