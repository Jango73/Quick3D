
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CTrajectorable.h"

class C3DScene;

class QUICK3D_EXPORT CMan : public CTrajectorable
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CMan(C3DScene* pScene);

	//!
	virtual ~CMan();

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CMan; }

	//!
	virtual void loadParameters(CXMLNode xComponent);

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------
};
