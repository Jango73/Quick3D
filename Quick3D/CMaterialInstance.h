
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CMaterial.h"

class QUICK3D_EXPORT CMaterialInstance
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CMaterialInstance(CMaterial* pMaterial);

	//!
	virtual ~CMaterialInstance();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne le mat�riau r�f�renc�
	CMaterial* material() const { return m_pMaterial; }

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de la classe
	virtual QString getClassName() const { return ClassName_CMaterialInstance; }

protected:

	CMaterial*	m_pMaterial;
};
