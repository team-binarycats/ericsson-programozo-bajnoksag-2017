@0x8a4fdb40485958dd;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("ericsson2017::protocol::test");


struct Bugfix {
    bugs @0 : UInt8;
    message @1 : Text;
}

