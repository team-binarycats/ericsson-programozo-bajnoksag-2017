#include "main.h"
#include "interface.h"
#include "messaging.h"

#include <iostream> // For usage message

using namespace ericsson2017::protocol;

char* hash;
char* username;

int main(int argc, char* argv[]) {
	Response::Reader currentResponse;
	int lastLevel = -1;

	if (argc != 3) {
		std::cerr<<"Usage: "<<argv[0]<<" username hash"<<std::endl;
		return 1;
	} else {
		username = argv[1];
		log((std::string)"Using username "+(std::string)username);

		hash = argv[2];
		log((std::string)"Using hash "+(std::string)hash);
	}

	login([](Command::Login::Builder login){
		login.setTeam(username);
		login.setHash(hash);
		log("Sending login information...");
	});
	while (true) {
		response([&currentResponse](const Response::Reader& response){
			draw_response(response);
			currentResponse=response;
		}, write_status);
		if ( currentResponse.getInfo().getLevel() != lastLevel ) {
			log((std::string)"Setting up for level "+std::to_string(currentResponse.getInfo().getLevel()));
			setup(currentResponse.getInfo().getLevel());
			lastLevel = currentResponse.getInfo().getLevel();
		}
		request([currentResponse](Move::Builder& move){
			loop(currentResponse, move);
		});
	}

	return 0;
}
