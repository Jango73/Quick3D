
#pragma once

// qt-plus
#include "CSingleton.h"
#include "CXMLNode.h"

// Application
#include "quick3d_global.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponentLoader : public CSingleton<CComponentLoader>
{
    friend class CSingleton<CComponentLoader>;

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

protected:

    //! Default constructor
    CComponentLoader();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

public:

    //! Loads a scene
    QVector<QSP<CComponent> > load(const QString& sBaseFile, C3DScene* pScene);

    //! Loads a component
    CComponent* loadComponent(const QString& sBaseFile, C3DScene* pScene);

    //! Loads a component
    CComponent* loadComponent(const QString& sBaseFile, C3DScene* pScene, CXMLNode xComponent, CComponent* pParent);
};
