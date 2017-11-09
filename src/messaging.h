#pragma once

#include "Command.capnp.h"
#include "Response.capnp.h"

#include <functional>

namespace ericsson2017 { namespace protocol {
	const int receive_fd = 3;
	const int send_fd = 4;

	void login(std::function<void(Command::Login::Builder)> login_handler);
	void response(std::function<void(const Response::Reader&)> response_handler, std::function<void(const ::capnp::Text::Reader&)> status_handler);
	void request(std::function<void(Move::Builder&)> move_handler);
}} // Namespaces
