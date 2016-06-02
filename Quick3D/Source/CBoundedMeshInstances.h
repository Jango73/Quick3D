
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CMeshInstance.h"

class C3DScene;

class QUICK3D_EXPORT CBoundedMeshInstances : public CComponent
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CBoundedMeshInstances(C3DScene* pScene);

	//!
	~CBoundedMeshInstances();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setBounds(CBoundingBox bBox);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	const QVector<CMeshInstance*>& getMeshes() { return m_vMeshes; }

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CBoundedMeshInstances; }

	//!
	virtual CBoundingBox getBounds() const;

	//!
	virtual CBoundingBox getWorldBounds() const;

	//!
	virtual void update(double dDeltaTime);

	//! Renders the mesh
	virtual void paint(CRenderContext* pContext);

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//! Ajoute un mesh dans ce d�limiteur
	void add(CMeshInstance* pMeshInstance);

protected:

	CBoundingBox				m_bBounds;
	QVector<CMeshInstance*>		m_vMeshes;
};
