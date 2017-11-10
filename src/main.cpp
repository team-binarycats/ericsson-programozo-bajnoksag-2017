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
	std::function<void()> fetch_and_proc_response;
	auto response_handler = [&fetch_and_proc_response, &lastLevel, &lastHealth](const Response::Reader& currentResponse) {
		if ( currentResponse.getInfo().getLevel() != lastLevel ) {
			setup(currentResponse.getInfo().getLevel());
			lastLevel = currentResponse.getInfo().getLevel();
		}
		request([currentResponse](Move::Builder& move){
			loop(currentResponse, move);
		});
		fetch_and_proc_response();
	};
	fetch_and_proc_response = [&response_handler](){
		response([&response_handler](const Response::Reader& response){
			draw_response(response);
			response_handler(response);
		}, write_status);
	};

	fetch_and_proc_response();
	return 0;
}
