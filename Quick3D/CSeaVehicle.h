
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CVehicle.h"

class C3DScene;

class QUICK3D_EXPORT CSeaVehicle : public CVehicle
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CSeaVehicle(C3DScene* pScene);

	//!
	virtual ~CSeaVehicle();

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CSeaVehicle; }
};
