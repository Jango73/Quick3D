
#pragma once

#include "quick3d_global.h"

// Foundations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CAxis.h"
#include "CSingleton.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPerlin : public CSingleton<CPerlin>
{
    friend class CSingleton<CPerlin>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CPerlin();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns a noise value for a given position
    double noise(Math::CVector3 pos);

    //! Returns a normalized noise value for a given position
    double noise_0_1(Math::CVector3 pos);

    //! Returns a turbulence value for a given position
    double turbulence(Math::CVector3 pos);

    //! Returns an erosion value for a given position
    double erosion(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Returns a Voronoi pattern value for a given position
    double voronoi(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

    //! Returns a displacement value for a given position
    Math::CVector3 displace(Math::CVector3 pos, double scale);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CMatrix4  m_mNoiseRotation;
};
