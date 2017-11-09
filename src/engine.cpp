#include "engine.h"

#include <exception>

using namespace ericsson2017::protocol;

Response::Reader ::ericsson2017::protocol::lastResponse;

void ::ericsson2017::protocol::save_response(const Response::Reader& response) {
	lastResponse = response;
}
