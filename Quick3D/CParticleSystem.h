
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CMesh.h"
#include "CParticle.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CParticleSystem : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CParticleSystem(C3DScene* pScene);

	//! Destructeur
	virtual ~CParticleSystem();

	//!
	virtual void update(double dDeltaTime);

	//!
	virtual void paint(CRenderContext* pContext);

	//!
	QVector<CParticle>& getParticles() { return m_vParticles; }

protected:

	QVector<CParticle>	m_vParticles;
};
