@0xabf8f53c554fa923;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("ericsson2017::protocol");


using Common = import "Common.capnp";


struct Move {
    unit @0 : Int32;
    direction @1 : Common.Direction;
}

struct Command {
    union {
        moves @0 : List(Move);
        login : group {
            team @1 : Text;
            hash @2 : Text;
        }
    }
}
