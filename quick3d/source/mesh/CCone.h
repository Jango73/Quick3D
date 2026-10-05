
#pragma once

// Application
#include "quick3d_global.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCone : public CMesh
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor using a scene
    CCone(C3DScene* pScene, double dMaxDistance, int iNumSegments, int iNumHeightSegments, double dHeight, double dBaseRadius, double dApexRadius);

    //! Destructor
    virtual ~CCone();
};
