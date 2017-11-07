#include "interface.h"

using namespace ericsson2017::protocol::test;

void ::ericsson2017::protocol::test::log(::std::string message) {
	::std::cerr<<"* "<<message<<::std::endl;
}
