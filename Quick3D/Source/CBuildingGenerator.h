
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
    // Constructors and destructor
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
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex          m_mMutex;
    C3DScene*       m_pScene;

    // Shared data

    QSP<CMesh>      m_pBuilding1;
};
