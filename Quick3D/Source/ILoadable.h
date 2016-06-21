
#pragma once

#include "CXMLNode.h"

class C3DScene;

class ILoadable
{
public:

    //! Charge les propriétés de l'objet depuis un CXMLNode
    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) = 0;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) = 0;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) = 0;
};
