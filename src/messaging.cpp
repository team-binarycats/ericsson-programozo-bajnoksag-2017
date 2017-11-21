#include "messaging.h"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace ericsson2017 { namespace protocol {

void login(std::function<void(Command::Commands::Login::Builder)> login_handler) {
	::capnp::MallocMessageBuilder message;
	Command::Builder command = message.initRoot<Command>();
	Command::Commands::Builder commands = command.initCommands();

	if (login_handler != nullptr) login_handler(commands.initLogin());

	::capnp::writePackedMessageToFd(send_fd, message);
}

void response(std::function<void(const Response::Reader&)> response_handler, std::function<void(const ::capnp::Text::Reader&)> status_handler) {
	::capnp::PackedFdMessageReader msg(receive_fd);
	Response::Reader resp = msg.getRoot<Response>();
	
	if (status_handler != nullptr) status_handler(resp.getStatus());
	if (response_handler != nullptr) response_handler(resp);
}

void request(std::function<void(Move::Builder&)> move_handler) {
	::capnp::MallocMessageBuilder message;
	Command::Builder command = message.initRoot<Command>();
	Command::Commands::Builder commands = command.initCommands();

	if (move_handler != nullptr) {
		auto moves = commands.initMoves(1);
		auto move = moves[0];
		move_handler(move);
	}

	::capnp::writePackedMessageToFd(send_fd, message);
}

}} // Namespaces
