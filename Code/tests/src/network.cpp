#include "catch.hpp"

#include "Socket.h"

TEST_CASE("Networking", "[network]")
{
    InitializeNetwork();

    Socket sock;

    ShutdownNetwork();
}