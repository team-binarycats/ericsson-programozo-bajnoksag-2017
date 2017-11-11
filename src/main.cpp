#include "main.h"
#include "interface.h"
#include "messaging.h"

#include <string> // For usage message

using namespace ericsson2017::protocol;

char* hash;
char* username;

int main(int argc, char* argv[]) {
	Response::Reader currentResponse;
	int lastLevel = -1;
	int lastHealth = 3;

	if (argc != 3) {
		log("Skipping login");
		log((std::string)"Basic usage: "+(std::string)argv[0]+(std::string)" <username> <hash>");
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

	std::function<void()> fetch_and_proc_response;
	auto response_handler = [&fetch_and_proc_response, &lastLevel, &lastHealth](const Response::Reader& currentResponse) {
		if ( currentResponse.getInfo().getLevel() != lastLevel ) {
			if ( lastLevel == -1 ) {
				log((std::string)"Setting up for level "+std::to_string(currentResponse.getInfo().getLevel()));
				setup(currentResponse.getInfo().getLevel(), SetupReason::INIT);
			} else if ( currentResponse.getInfo().getLevel() < lastLevel ) {
				log("Lost that level :(");
				setup(currentResponse.getInfo().getLevel(), SetupReason::LEVELDOWN);
			} else { // level > lastLevel
				log("Level up, good job!");
				setup(currentResponse.getInfo().getLevel(), SetupReason::LEVELUP);
			}
			lastLevel = currentResponse.getInfo().getLevel();
		} else if ( ( currentResponse.getUnits().size() == 1 ) && ( currentResponse.getUnits()[0].getHealth() != lastHealth ) ) {
			log("Health differs, This should mean something!");
			if ( currentResponse.getUnits()[0].getHealth() < lastHealth ) {
				log("Unit died :(");
				setup(currentResponse.getInfo().getLevel(), SetupReason::DEATH);
			} else { // health < lastHealth
				log("Unit healed. What a suprise!");
				setup(currentResponse.getInfo().getLevel(), SetupReason::HEAL);
			}
			lastHealth = currentResponse.getUnits()[0].getHealth();
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
