
// Application
#include "CParticle.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CParticle::CParticle()
{
}

//-------------------------------------------------------------------------------------------------

CParticle::CParticle(CVector3 NewPosition, CVector3 NewVelocity)
{
    m_vPosition	= NewPosition;
    m_vVelocity = NewVelocity;
}

//-------------------------------------------------------------------------------------------------

CParticle& CParticle::operator = (const CParticle& target)
{
    m_vPosition	= target.m_vPosition;
    m_vVelocity	= target.m_vVelocity;

    return *this;
}
