
#pragma once

#include "quick3d_global.h"

// Application
#include "CMeshInstance.h"

class QUICK3D_EXPORT CBuildingGenerator
{
public:

    enum EBuildingType
    {
        BT_None,
        BT_Skyscraper,
        BT_MiddleSized,
        BT_House
    };

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //!
    CBuildingGenerator(C3DScene* pScene);

    //!
    ~CBuildingGenerator();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    CMeshInstance* getBuilding(EBuildingType eType, CGeoloc gPosition, double dRadius, double dSeed = 0.0);

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex                  m_mMutex;
    C3DScene*               m_pScene;

    // Shared data

    QSharedPointer<CMesh>   m_pBuilding1;
};
