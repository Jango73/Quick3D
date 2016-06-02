
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"
#include "CXMLNode.h"

#include "CHeightField.h"
#include "CBILData.h"

class QUICK3D_EXPORT CBILField : public CHeightField
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur avec paramètres
	CBILField(CXMLNode xParameters, const QString& sPathToBILFiles = "", double dValueForNoData = 0.0);

	//! Destructeur
	virtual ~CBILField();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	virtual double getHeightAt(const Math::CGeoloc& gPosition, double* pRigidness = NULL);

	//!
	virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness = NULL);

	//!
	virtual double getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics = true);

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	virtual void flatten(const Math::CGeoloc& gPosition, double dRadius);

	//!
	void addChunk(CBILData* pData);

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//!
	void parseBILFiles();

	//!
	void collectGarbage();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	QMutex				m_tMutex;
	CXMLNode			m_xParameters;
	double				m_dValueForNoData;
	QString				m_sPath;
	QVector<CBILData*>	m_vChunks;
};
