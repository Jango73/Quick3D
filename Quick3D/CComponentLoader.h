
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

	//! Constructeur par d�faut
	CComponentLoader();

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

public:

	//! Charge une sc�ne
	QVector<CComponent*> load(C3DScene* pScene, QString sFileName);

	//! Charge un composant
	CComponent* loadComponent(C3DScene* pScene, QString sFileName);

	//! Charge un composant
	CComponent* loadComponent(C3DScene* pScene, CXMLNode xComponent, CComponent* pParent);
};
