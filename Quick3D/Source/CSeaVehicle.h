
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CVehicle.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

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
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CSeaVehicle; }
};
