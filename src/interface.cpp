#include "interface.h"

using namespace ericsson2017::protocol;

void ::ericsson2017::protocol::log(::std::string message) {
	::std::cerr<<"* "<<message<<::std::endl;
}

void ::ericsson2017::protocol::write_status(const ::capnp::Text::Reader& status) {
	::std::cerr<<"] ";
	::std::cerr<<(::std::string)status<<::std::endl;
}
