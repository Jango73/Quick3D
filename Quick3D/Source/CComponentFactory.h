
#pragma once

// Qt
#include <QString>
#include <QVector>

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"
#include "ComponentInstanciator.h"

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

    //! Enregistre les composants du noyau
    void registerCoreComponents();

    //! Enregistre un composant
    void registerComponent(QString sClassName, MComponentInstanciator pInstanciator);

    //! Instancie un composant selon son nom de classe pour la scène donnée
    CComponent* instanciateComponent(QString sClassName, C3DScene* pScene);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    static QMap<QString, MComponentInstanciator>    s_vComponentInstanciators;
};
