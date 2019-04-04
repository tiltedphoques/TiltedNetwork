#include "catch.hpp"
#include "Outcome.h"
#include "StandardAllocator.h"
#include "BoundedAllocator.h"
#include <string>

TEST_CASE("Outcome saves the result and errors", "[core.outcome]")
{
    SECTION("Clean")
    {
        Outcome<int, std::string> outcome;
        REQUIRE(outcome.HasError());
    }

    SECTION("Result")
    {
        Outcome<int, std::string> outcome(42);
        REQUIRE_FALSE(outcome.HasError());
        REQUIRE(outcome.GetResult() == 42);
        REQUIRE(outcome.GetError() == std::string{});

        Outcome<int, std::string> outcomeCopy{ outcome };
        REQUIRE_FALSE(outcomeCopy.HasError());
        REQUIRE(outcomeCopy.GetResult() == 42);
        REQUIRE(outcomeCopy.GetError() == std::string{});

        Outcome<int, std::string> outcomeMove{ std::move(outcome) };
        REQUIRE_FALSE(outcomeMove.HasError());
        REQUIRE(outcomeMove.GetResult() == 42);
        REQUIRE(outcomeMove.GetError() == std::string{});
    }

    SECTION("Error")
    {
        Outcome<int, std::string> outcome("hello");
        REQUIRE(outcome.HasError());
        REQUIRE(outcome.GetError() == std::string{ "hello" });

        Outcome<int, std::string> outcomeCopy{ outcome };
        REQUIRE(outcomeCopy.HasError());
        REQUIRE(outcomeCopy.GetError() == std::string{ "hello" });

        Outcome<int, std::string> outcomeMove{ std::move(outcome) };
        REQUIRE(outcomeMove.HasError());
        REQUIRE(outcomeMove.GetError() == std::string{ "hello" });
    }
}

TEST_CASE("Allocators allocate memory", "[core.allocators]")
{
    GIVEN("A StandardAllocator")
    {
        StandardAllocator allocator;

        auto pData = allocator.Allocate(100);
        REQUIRE(pData != nullptr);
        REQUIRE(allocator.Size(pData) == 100);
        allocator.Free(pData);
    }

    GIVEN("A BoundedAllocator")
    {
        BoundedAllocator allocator(1000);
        WHEN("Capacity is sufficient")
        {
            auto pData = allocator.Allocate(1000);
            REQUIRE(pData != nullptr);
            REQUIRE(allocator.Size(pData) == 1000);
            allocator.Free(pData);

            pData = allocator.Allocate(1000);
            REQUIRE(pData != nullptr);
            REQUIRE(allocator.Size(pData) == 1000);
            allocator.Free(pData);

            pData = allocator.Allocate(900);
            REQUIRE(pData != nullptr);
            REQUIRE(allocator.Size(pData) == 900);

            auto pDataBis = allocator.Allocate(100);
            REQUIRE(pDataBis != nullptr);
            REQUIRE(allocator.Size(pDataBis) == 100);

            allocator.Free(pData);
            allocator.Free(pDataBis);
        }
        WHEN("Capacity is insufficient")
        {
            auto pData = allocator.Allocate(1001);
            REQUIRE(pData == nullptr);

            pData = allocator.Allocate(900);
            REQUIRE(pData != nullptr);
            REQUIRE(allocator.Size(pData) == 900);

            auto pDataBis = allocator.Allocate(101);
            REQUIRE(pDataBis == nullptr);

            allocator.Free(pData);
        }
    }
}