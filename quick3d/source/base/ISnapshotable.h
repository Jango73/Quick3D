
#pragma once
// Application
#include "quick3d_global.h"

// Qt
#include <QString>

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT ISnapshotable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Destructor
    virtual ~ISnapshotable() {}

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns a JSON document with the object's state for the current frame
    virtual QString snapshotJson() = 0;
};
