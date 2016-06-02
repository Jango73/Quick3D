
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CVehicle.h"

class C3DScene;

class QUICK3D_EXPORT CTerrestrialVehicle : public CVehicle
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CTerrestrialVehicle(C3DScene* pScene);

	//!
	virtual ~CTerrestrialVehicle();

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CTerrestrialVehicle; }

	//!
	virtual void loadParameters(CXMLNode xComponent);

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------
};
