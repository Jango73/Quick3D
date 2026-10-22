
#pragma once

// Application
#include "quick3d_global.h"

// qt-plus
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT ILoadable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Destructor
    virtual ~ILoadable() {}

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) = 0;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) = 0;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) = 0;
};
