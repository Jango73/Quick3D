
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
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CSeaVehicle(C3DScene* pScene);

	//!
	virtual ~CSeaVehicle();

	//-------------------------------------------------------------------------------------------------
	// Inherited methods
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CSeaVehicle; }
};
