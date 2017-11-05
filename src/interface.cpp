#include "interface.h"

using namespace ericsson2017::protocol::test;

void ::ericsson2017::protocol::test::log(::std::string message) {
	::std::cerr<<"* "<<message<<::std::endl;
}

void ::ericsson2017::protocol::test::write_bugfix(const Bugfix::Reader& bugfix) {
	::std::cerr<<"> ";
	::std::cout<<(int)bugfix.getBugs()<<" "<<(::std::string)bugfix.getMessage()<<::std::endl;
	if (!isatty(fileno(stdout))) ::std::cerr<<"#"<<(int)bugfix.getBugs()<<": \""<<(::std::string)bugfix.getMessage()<<"\""<<::std::endl;
}

void ::ericsson2017::protocol::test::write_status(const ::capnp::Text::Reader& status) {
	::std::cerr<<"] ";
	::std::cerr<<(::std::string)status<<::std::endl;
}

void ::ericsson2017::protocol::test::read_bugfix(Bugfix::Builder& bugfix) {
	int bugs;
	::std::string message;

	::std::cerr<<"< ";
	::std::cin>>bugs;
	::std::cin.ignore(1);
	::std::getline(::std::cin, message);

	if (::std::cin.eof()) {
		::std::cerr<<::std::endl;
		log("End of input before got an end response");
		::std::cerr<<::std::endl;
		throw ::std::runtime_error("End of input");
	}

	bugfix.setBugs(bugs);
	bugfix.setMessage(message);

	if (!isatty(fileno(stdin))) ::std::cerr<<"#"<<(int)bugs<<": \""<<(::std::string)message<<"\""<<::std::endl;
}
