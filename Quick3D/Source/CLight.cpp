
// Application
#include "CLight.h"

//-------------------------------------------------------------------------------------------------

CLight::CLight(C3DScene* pScene)
: CCamera(pScene)
, m_dDistance(0.0)
, m_bCastShadows(false)
, m_pMaterial(NULL)
{
	setRaytracable(false);

	m_pMaterial = new CMaterial(pScene);
	m_pMaterial->createShadowTexture();
}

//-------------------------------------------------------------------------------------------------

CLight::CLight(const CLight& target)
: CCamera(target)
, m_pMaterial(NULL)
{
	*this = target;
}

//-------------------------------------------------------------------------------------------------

CLight::~CLight()
{
	if (m_pMaterial != NULL)
	{
		delete m_pMaterial;
	}
}

//-------------------------------------------------------------------------------------------------

CLight& CLight::operator = (const CLight& target)
{
	CCamera::operator = (target);

	m_dDistance			= target.m_dDistance;
	m_bCastShadows		= target.m_bCastShadows;

	return *this;
}
