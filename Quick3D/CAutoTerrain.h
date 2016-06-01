
#pragma once

// Qt
#include <QImage>
#include <QDateTime>

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CWorldChunk.h"
#include "CHeightField.h"
#include "CGenerateFunction.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVegetation
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	CVegetation()
		: m_dSpread(0.0)
		, m_pFunction(NULL)
		, m_pMesh(NULL)
	{
	}

	CVegetation(double dSpread, CGenerateFunction* pFunction, CMeshInstance* pMesh)
		: m_dSpread(dSpread)
		, m_pFunction(pFunction)
		, m_pMesh(pMesh)
	{
	}

	~CVegetation()
	{
		delete m_pFunction;
		delete m_pMesh;
	}

	double				m_dSpread;
	CGenerateFunction*	m_pFunction;
	CMeshInstance*		m_pMesh;
};

class QUICK3D_EXPORT CAutoTerrain : public CComponent, public CHeightField
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CAutoTerrain(C3DScene* pScene, Math::CGeoloc gCameraPosition = Math::CGeoloc(), CHeightField* pHeights = NULL, bool bGenerateNow = false);

	//!
	virtual ~CAutoTerrain();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setTerrainResolution(int value);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	CHeightField* getHeights() { return m_pHeights; }

	//!
	QVector<CVegetation*>& getVegetation() { return m_vVegetation; }

	//!
	CMaterial* getMaterial() { return m_pMaterial.data(); }

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CAutoTerrain; }

	//!
	virtual void loadParameters(CXMLNode xComponent);

	//!
	virtual void paint(CRenderContext* pContext);

	//! Méthode de mise à jour avec temps delta depuis la dernière itération
	virtual void update(double dDeltaTime);

	//! Méthode de mise à jour avec temps delta depuis la dernière itération
	virtual void postUpdate(double dDeltaTime);

	//!
	virtual double getHeightAt(const Math::CGeoloc& gPosition, double* pRigidness = NULL);

	//!
	virtual void flatten(const Math::CGeoloc& gPosition, double dRadius);

	//! Calcul d'intersection avec un rayon
	virtual Math::RayTracingResult intersect(Math::CRay3 ray) const;

	//! Dump du contenu dans un flux
	virtual void dump(QTextStream& stream, int iIdent);

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégés
	//-------------------------------------------------------------------------------------------------

protected:

	//!
	void buildRoot();

	//!
	void paintRecurse(QVector<CWorldChunk*>& vChunkCollect, CRenderContext* pContext, CWorldChunk* pChunk, int iLevel, bool bForcePaint);

	//!
	void buildRecurse(CWorldChunk* pChunk, CRenderContext* pContext, int iLevel);

	//!
	double getHeightAtRecurse(const Math::CGeoloc& gPosition, CWorldChunk* pChunk, double* pRigidness = NULL);

	//!
	void collectGarbage();

	//!
	void collectGarbageRecurse(CWorldChunk* pChunk);

	//!
	void generateVegetation();

	//! Calcul d'intersection avec un rayon
	Math::RayTracingResult intersectRecurse(CWorldChunk* pChunk, const Math::CRay3& ray) const;

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	bool						m_bGenerateNow;
	CWorldChunk*				m_pRoot;
	CHeightField*				m_pHeights;
	int							m_iLevels;
	int							m_iTerrainResolution;
	CXMLNode					m_xParameters;
	QSharedPointer<CMaterial>	m_pMaterial;
	QVector<CVegetation*>		m_vVegetation;
};
