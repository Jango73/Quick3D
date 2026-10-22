
#pragma once

// Qt
#include <QString>
#include <QStringList>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CJsonBuilder
{
public:

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Joins pre-rendered JSON values into an array document: [v1,v2,...]
    static QString joinArray(const QStringList& vItems);

    //! Returns the success envelope with no payload: {"ok":true}
    static QString okResponse();

    //! Returns the failure envelope: {"ok":false,"error":"..."}
    static QString errorResponse(const QString& sError);

    //! Returns the failure envelope from a literal: {"ok":false,"error":"..."}
    static QString errorResponse(const char* pError);
};
