@0xc037d851bb528efa;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("ericsson2017::protocol::test");

using import "Bugfix.capnp".Bugfix;


struct Response {
    status @0 : Text;
    union {
        bugfix @1 : Bugfix;
        end @2 : Bool;
    } 
}
