
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CGeoloc.h"
#include "CAxis.h"
#include "CRay3.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CHeightField(double dRigidness = 1.0);

    //!
    virtual ~CHeightField();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the altitude at the specified geolocation
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = nullptr);

    //! Returns the altitude at the specified geolocation
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = nullptr);

    //! Returns the altitude at the specified geolocation
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //! Returns the terrain rigidness at the specified geolocation
    double getRigidness() const { return m_dRigidness; }

    //! Returns \c true if the field is ready for use
    virtual bool isGenerated();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius_m);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double  m_dRigidness;
};
