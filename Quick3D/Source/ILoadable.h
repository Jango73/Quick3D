
#pragma once

#include "CXMLNode.h"

class ILoadable
{
public:

    //! Charge les propriétés de l'objet depuis un CXMLNode
    //! Loads the object's properties from a CXMLNode
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) = 0;
};
