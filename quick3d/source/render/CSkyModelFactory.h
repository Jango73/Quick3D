
#pragma once

// Qt
#include <QString>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

class CSkyModel;

//-------------------------------------------------------------------------------------------------

/*!
    Factory of sky rendering models.

    Builds the model matching a scene XML name. Lives in its own files so the
    CSkyModel base class never includes its derived classes.
*/
class QUICK3D_EXPORT CSkyModelFactory
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Creates the model matching \a sName, null when unknown
    static QSP<CSkyModel> create(const QString& sName);
};
