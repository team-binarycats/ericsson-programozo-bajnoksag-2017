/** Client for Ericsson Programozó Bajnokság 2017.
 * Currently it just connects to a given server using a given username and hash
 */

#include "interface.h"
#include "engine.h"
#include "messaging.h"

#include "Command.capnp.h"
#include "Response.capnp.h"

#include <string>
#include <iostream> // For usage message

using namespace ericsson2017::protocol;

char* hash;
char* username;
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

	login([](Command::Login::Builder login){
		login.setTeam(username);
		login.setHash(hash);
		log("Sending login information...");
	});
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
