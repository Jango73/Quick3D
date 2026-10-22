
#pragma once
// Application
#include "quick3d_global.h"

// Qt
#include <QString>

// qt-plus
#include "CXMLNode.h"

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

    //! Returns the object state for the current frame as a node, JSON through CXMLNode
    virtual CXMLNode snapshotNode() = 0;
};
