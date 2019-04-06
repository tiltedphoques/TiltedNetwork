#include "catch.hpp"

#include "Outcome.h"
#include "StandardAllocator.h"
#include "BoundedAllocator.h"
#include "ScratchAllocator.h"
#include "StackAllocator.h"

#include <string>
#include <thread>
#include <future>

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

        WHEN("Using new/delete")
        {
            auto pInteger = allocator.New<uint32_t>();
            REQUIRE(pInteger != nullptr);
            allocator.Delete(pInteger);

            struct Dummy
            {
                Dummy(int& aValue) : m_value(aValue) { m_value++; }
                ~Dummy() { m_value++; }
                int& m_value;
            };

            int value = 0;
            auto pDummy = allocator.New<Dummy>(std::ref(value));
            REQUIRE(pDummy != nullptr);
            REQUIRE(value == 1);
            allocator.Delete(pDummy);
            REQUIRE(value == 2);
        }
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

TEST_CASE("Making sure allocator stacks work corrently", "[core.allocator.stack]")
{
    GIVEN("No allocator has been pushed")
    {
        REQUIRE(Allocator::Get() != nullptr);
        auto pAllocation = Allocator::Get()->Allocate(100);
        REQUIRE(pAllocation != nullptr);
        Allocator::Get()->Free(pAllocation);

        WHEN("Pushing an allocator")
        {
            BoundedAllocator allocator(1000);
            Allocator::Push(&allocator);
            REQUIRE(Allocator::Get() == &allocator);
            auto futureResult = std::async(std::launch::async, []() { return Allocator::Get(); });
            REQUIRE(futureResult.get() != &allocator);
            REQUIRE(Allocator::Pop() == &allocator);
            REQUIRE(Allocator::Get() != &allocator);
        }
        WHEN("Using allocators indirectly")
        {
            GIVEN("A pod type")
            {
                auto pValue = New<int>(42);
                REQUIRE(pValue != nullptr);
                REQUIRE(*pValue == 42);
            }

            GIVEN("An AllocatorCompatible type")
            {
                struct Dummy : AllocatorCompatible
                {
                    Dummy()
                    {
                        REQUIRE(GetAllocator() != nullptr);
                        auto pData = GetAllocator()->Allocate(100);
                        REQUIRE(pData != nullptr);
                        GetAllocator()->Free(pData);
                    }

                    ~Dummy() {}
                };

                auto pDummy = New<Dummy>();
                Delete(pDummy);
            }
        }
    }
}

TEST_CASE("Using a scratch allocator", "[core.allocator.scratch]")
{
    ScratchAllocator allocator(1000);

    REQUIRE(allocator.Size(nullptr) == 1000);

    void* pResult = allocator.Allocate(10);
    REQUIRE(pResult != nullptr);
    pResult = allocator.Allocate(1000);
    REQUIRE(pResult == nullptr);
}

TEST_CASE("Allocating memory on the stack", "[core.allocator.stack]")
{
    StackAllocator<1000> allocator;
    REQUIRE(allocator.Size(nullptr) == 1000);

    auto pResult = allocator.Allocate(100);
    REQUIRE(pResult != nullptr);
    REQUIRE(allocator.Allocate(1000) == nullptr);
}