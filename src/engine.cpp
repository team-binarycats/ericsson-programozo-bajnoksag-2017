#include "engine.h"

#include <exception>

namespace ericsson2017 { namespace protocol {

Response::Reader lastResponse;

void save_response(const Response::Reader& response) {
	//lastResponse = response; // Not working (copies as pointer)
}

}} // Namespaces
