#pragma once

#include <string>

#include "Response.capnp.h"

namespace ericsson2017 { namespace protocol {
	void log(::std::string message);
	void write_status(const ::capnp::Text::Reader& status);
	void draw_response(const Response::Reader& response);
} } // Namespaces
