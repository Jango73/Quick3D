
#pragma once

#include "quick3d_global.h"

// Application
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CBox : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur d'après une scène
    CBox(C3DScene* pScene, double dMaxDistance = 10000.0);

	//! Destructeur
	virtual ~CBox();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setMinimum(const Math::CVector3& vMinimum);

	//!
	void setMaximum(const Math::CVector3& vMaximum);

	//!
	void setBounds(const Math::CVector3& vMinimum, const Math::CVector3& vMaximum);

protected:

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

	//!
	void fillVertices();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3		m_vMinimum;
	Math::CVector3		m_vMaximum;
};
