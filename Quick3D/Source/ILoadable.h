
#pragma once

#include "CXMLNode.h"

class C3DScene;

class ILoadable
{
public:

    //! Charge les propriétés de l'objet depuis un CXMLNode
    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) = 0;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) = 0;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) = 0;
};
