
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCone : public CMesh
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructeur d'après une scène
    CCone(C3DScene* pScene, double dMaxDistance, int iNumSegments, int iNumHeightSegments, double dHeight, double dBaseRadius, double dApexRadius);

    //! Destructor
    virtual ~CCone();
};
