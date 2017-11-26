#include "messaging.h"

using namespace ericsson2017::protocol;

char* hash;
char* username;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		return 0;
	} else {
		username = argv[1];
		log((std::string)"Using username "+(std::string)username);

		hash = argv[2];
		log((std::string)"Using hash "+(std::string)hash);

		login([](Command::Commands::Login::Builder login){
			login.setTeam(username);
			login.setHash(hash);
			log("Sending login information...");
		});
	}
	
}
