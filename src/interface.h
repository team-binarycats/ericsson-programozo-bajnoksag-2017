#pragma once

#include "Request.capnp.h"
#include "Response.capnp.h"
#include "Bugfix.capnp.h"

#include <iostream>
#include <unistd.h>
#include <exception>
#include <string>

namespace ericsson2017 { namespace protocol { namespace test {
	void log(::std::string message);
	void write_bugfix(const Bugfix::Reader& bugfix);
	void write_status(const ::capnp::Text::Reader& status);
	void read_bugfix(Bugfix::Builder& bugfix);
} } } // Namespaces
