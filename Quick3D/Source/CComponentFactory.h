
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QString>
#include <QVector>

#include "CSingleton.h"

// Application
#include "ComponentInstanciator.h"

class QUICK3D_EXPORT CComponentFactory : public CSingleton<CComponentFactory>
{
    friend class CSingleton<CComponentFactory>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CComponentFactory();

    //!
    virtual ~CComponentFactory();

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Enregistre les composants du noyau
    void registerCoreComponents();

    //! Enregistre un composant
    void registerComponent(QString sClassName, MComponentInstanciator pInstanciator);

    //! Instancie un composant selon son nom de classe pour la scène donnée
    CComponent* instanciateComponent(QString sClassName, C3DScene* pScene);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    static QMap<QString, MComponentInstanciator>	s_vComponentInstanciators;
};
