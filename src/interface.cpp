#include "interface.h"

using namespace ericsson2017::protocol;

void ::ericsson2017::protocol::log(::std::string message) {
	::std::cerr<<"* "<<message<<::std::endl;
}
