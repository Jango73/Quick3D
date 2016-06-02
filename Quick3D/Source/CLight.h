
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector2.h"
#include "CVector3.h"
#include "CMaterial.h"
#include "CCamera.h"

class C3DScene;

class QUICK3D_EXPORT CLight : public CCamera
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CLight(C3DScene* pScene);

	//!
	CLight(const CLight&);

	//!
	virtual ~CLight();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setCastShadows(bool value) { m_bCastShadows = value; }

	//!
	void setDistance(double value) { m_dDistance = value; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	CMaterial* getMaterial() const { return m_pMaterial; }

	//!
	bool castShadows() const { return m_bCastShadows; }

	//!
	double getDistance() const { return m_dDistance; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	CLight& operator = (const CLight& target);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	double			m_dDistance;
	bool			m_bCastShadows;
	CMaterial*		m_pMaterial;
};
