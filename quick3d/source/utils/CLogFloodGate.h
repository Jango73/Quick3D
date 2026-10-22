
#pragma once

// Qt
#include <QtGlobal>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CLogFloodGate
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Builds a gate passing \a iMaxHits hits per \a iCooldownMs window, then silence until the window expires
    CLogFloodGate(int iMaxHits = 3, int iCooldownMs = 60000);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns \c true when the caller may log: the first hits of each window pass, later calls are dropped
    bool allow();

    //! Forgets past hits, the next call passes
    void reset();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    int     m_iMaxHits; ///< Hits passing per window before silence
    int     m_iCooldownMs; ///< Window length in milliseconds
    int     m_iHits; ///< Hits consumed in the running window
    qint64  m_iWindowStartMs; ///< Start of the running window, epoch milliseconds
};
