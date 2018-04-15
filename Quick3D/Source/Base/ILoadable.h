
#pragma once

// qt-plus
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class ILoadable
{
public:

    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) = 0;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) = 0;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) = 0;
};
