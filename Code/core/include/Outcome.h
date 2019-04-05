#pragma once

#include <utility>

template <class Result, class Error>
class Outcome
{
public:

    Outcome()
        : m_failed{ true }
    {}

    Outcome(const Result& aResult)
        : m_failed{ false }
        , m_result{ aResult }
    {}

    Outcome(const Error& aError)
        : m_failed{ true }
        , m_error(aError)
    {}

    Outcome(Result&& aResult)
        : m_failed{ false }
        , m_result{ std::forward<Result>(aResult)}
    {}

    Outcome(Error&& aError)
        : m_failed{ true }
        , m_error{ std::forward<Error>(aError)}
    {}

    Outcome(const Outcome& aOutcome)
    {
        *this = aOutcome;
    }

    Outcome(Outcome&& aOutcome)
    {
        *this = std::move(aOutcome);
    }

    Outcome& operator=(const Outcome& aOutcome)
    {
        if (&aOutcome != this)
        {
            m_failed = aOutcome.m_failed;
            m_error = aOutcome.m_error;
            m_result = aOutcome.m_result;
        }

        return *this;
    }

    Outcome& operator=(Outcome&& aOutcome)
    {
        if (&aOutcome != this)
        {
            m_failed = std::move(aOutcome.m_failed);
            m_error = std::move(aOutcome.m_error);
            m_result = std::move(aOutcome.m_result);
        }

        return *this;
    }

    operator bool() const
    {
        return !HasError();
    }

    inline bool HasError() const
    {
        return m_failed;
    }

    inline const Error& GetError() const
    {
        return m_error;
    }

    inline const Result& GetResult() const
    {
        return m_result;
    }

    inline Result& GetResult()
    {
        return m_result;
    }

    inline Result&& MoveResult()
    {
        return std::move(m_result);
    }

private:

    bool m_failed;
    Result m_result;
    Error m_error;
};