
#pragma once

// Application
#include "quick3d_global.h"

// Qt
#include <QString>

//-------------------------------------------------------------------------------------------------

enum ELogLevel
{
	llDebug,
	llInfo,
	llWarning,
	llError,
	llCritical,
	llAlways
};

//-------------------------------------------------------------------------------------------------

//! Defines a logging interface
class QUICK3D_EXPORT ILogger
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Destructor
    virtual ~ILogger() {}

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    virtual void log(ELogLevel eLevel, const QString& sText, const QString& sToken = "") = 0;
    virtual void logBuffer(ELogLevel eLevel, const char* pBuffer, int iSize) = 0;
};
