
// Application
#include "CHeightField.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CHeightField::CHeightField(double dRigidness)
: m_dRigidness(dRigidness)
{
}

//-------------------------------------------------------------------------------------------------

CHeightField::~CHeightField()
{
}

//-------------------------------------------------------------------------------------------------

double CHeightField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
	if (pRigidness) *pRigidness = 0.0;

	return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CHeightField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
	if (pRigidness) *pRigidness = 0.0;

	return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CHeightField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
	return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

bool CHeightField::isGenerated()
{
	return false;
}

//-------------------------------------------------------------------------------------------------

void CHeightField::flatten(const CGeoloc& gPosition, double dRadius)
{
}
