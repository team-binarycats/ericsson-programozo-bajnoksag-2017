#pragma once

#include <iostream>
#include <unistd.h>
#include <exception>
#include <string>
#include <capnp/blob.h>

namespace ericsson2017 { namespace protocol {
	void log(::std::string message);
	void write_status(const ::capnp::Text::Reader& status);
} } // Namespaces
