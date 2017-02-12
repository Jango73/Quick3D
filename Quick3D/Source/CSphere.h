
#pragma once

#include "quick3d_global.h"

// Application
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CSphere : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//! Constructeur d'apr�s une sc�ne
	CSphere(C3DScene* pScene, double dMaxDistance, int iNumSegments);

	//! Destructor
	virtual ~CSphere();
};
