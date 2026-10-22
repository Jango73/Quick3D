
#pragma once

// Application
#include "quick3d_global.h"

// Qt
#include <QString>

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT IProgressListener
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Destructor
    virtual ~IProgressListener() {}

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Notifies the progress of an operation
    virtual void notifyProgress(QString sWhat, double dPercent) = 0;
};
