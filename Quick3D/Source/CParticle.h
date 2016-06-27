
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#include "CVector3.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CParticle
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par défaut
    CParticle();

    //! Constructeur avec paramètres
    CParticle(Math::CVector3 NewPosition, Math::CVector3 NewVelocity = Math::CVector3());

    //! Opérateur de copie
    CParticle& operator=(const CParticle& target);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    Math::CVector3& position() { return m_vPosition; }

    //!
    Math::CVector3 position() const { return m_vPosition; }

    //!
    Math::CVector3& velocity() { return m_vVelocity; }

    //!
    Math::CVector3 velocity() const { return m_vVelocity; }

protected:

    Math::CVector3	m_vPosition;
    Math::CVector3	m_vVelocity;
};
