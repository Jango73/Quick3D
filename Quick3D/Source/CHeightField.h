
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CAxis.h"
#include "CRay3.h"

class QUICK3D_EXPORT CHeightField
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
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

    //!
    virtual double getHeightAt(const CGeoloc& gPosition, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

    //!
    virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

    //!
    double getRigidness() const { return m_dRigidness; }

    //!
    virtual bool isGenerated();

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void flatten(const CGeoloc& gPosition, double dRadius);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    double			m_dRigidness;
};
