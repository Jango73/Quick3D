
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CAxis.h"
#include "CSingleton.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPerlin : public CSingleton<CPerlin>
{
	friend class CSingleton<CPerlin>;

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CPerlin();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une valeur de bruit pour une position donnée
	double getNoise(Math::CVector3 pos);

	//! Retourne une valeur de bruit normalisée pour une position donnée
	double getNoise_0_1(Math::CVector3 pos);

	//! Retourne une valeur de turbulence pour une position donnée
	double getTurbulence(Math::CVector3 pos);

	//! Retourne une valeur d'érosion pour une position donnée
	double getErosion(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

	//! Retourne une valeur du pattern de Voronoi pour une position donnée
	double getVoronoi(Math::CVector3 pos, Math::CAxis reference, double dDisplace);

	//! Retourne une valeur de déplacement pour une position donnée
	Math::CVector3 displace(Math::CVector3 pos, double scale);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CMatrix4	m_mNoiseRotation;
};
