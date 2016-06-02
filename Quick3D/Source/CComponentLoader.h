
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CSingleton.h"
#include "CXMLNode.h"

// Quick3D
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponentLoader : public CSingleton<CComponentLoader>
{
	friend class CSingleton<CComponentLoader>;

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

protected:

	//! Constructeur par défaut
	CComponentLoader();

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

public:

	//! Charge une scène
	QVector<CComponent*> load(C3DScene* pScene, QString sFileName);

	//! Charge un composant
	CComponent* loadComponent(C3DScene* pScene, QString sFileName);

	//! Charge un composant
	CComponent* loadComponent(C3DScene* pScene, CXMLNode xComponent, CComponent* pParent);
};
