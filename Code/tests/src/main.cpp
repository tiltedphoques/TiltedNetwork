#define CATCH_CONFIG_MAIN
#include "catch.hpp"

int main(int argc, char** argv)
{
    return Catch::Session().run(argc, argv);
}