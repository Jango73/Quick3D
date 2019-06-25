
#pragma once

// Qt
#include <QString>
#include <QVector>

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"
#include "ComponentInstantiator.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponentFactory : public CSingleton<CComponentFactory>
{
    friend class CSingleton<CComponentFactory>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CComponentFactory();

    //!
    virtual ~CComponentFactory();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Registers all core components
    void registerCoreComponents();

    //! Registers a single component
    void registerComponent(QString sClassName, MComponentInstantiator pInstantiator);

    //! Instantiates a component with the given class name, for the given scene
    CComponent* instantiateComponent(QString sClassName, C3DScene* pScene);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMap<QString, MComponentInstantiator>    m_vComponentInstantiators;
};
