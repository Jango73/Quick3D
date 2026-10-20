
#pragma once

//-------------------------------------------------------------------------------------------------

/*!
    Reports only value changes to keep debug output quiet. \br\br
    Generic flood control helper: any repeated computation (driver inputs,
    log tuples, sensor readouts) goes through this filter instead of keeping
    ad-hoc last value caches in callers. The first observed value always
    reports a change so initial state is emitted once.
*/
template<typename T>
class CChangeFilter
{

public:

    //! Builds an empty filter, no value observed yet
    CChangeFilter()
        : m_bValid(false)
    {
    }

    //! Returns true once when \a tValue differs from the last observed value
    bool hasChanged(const T& tValue)
    {
        if (m_bValid == false || m_tLast != tValue)
        {
            m_tLast = tValue;
            m_bValid = true;

            return true;
        }

        return false;
    }

    //! Forgets the last observed value, the next value reports a change
    void reset()
    {
        m_bValid = false;
    }

private:

    T       m_tLast;
    bool    m_bValid;
};

//-------------------------------------------------------------------------------------------------
