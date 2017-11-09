#pragma once

#include "Response.capnp.h"

namespace ericsson2017 { namespace protocol {
	extern Response::Reader lastResponse;
	void save_response(const Response::Reader& response);
} } // Namespaces
