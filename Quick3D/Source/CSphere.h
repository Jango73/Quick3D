
#pragma once

#include "quick3d_global.h"

// Application
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CSphere : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur d'après une scène
	CSphere(C3DScene* pScene, double dMaxDistance, int iNumSegments);

	//! Destructeur
	virtual ~CSphere();
};
