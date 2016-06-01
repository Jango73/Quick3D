
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CSingleton.h"

// Application
#include "CMesh.h"

class QUICK3D_EXPORT COBJLoader : public CSingleton<COBJLoader>
{
	friend class CSingleton<COBJLoader>;

public:

	//! Charge un fichier .obj
	void load(C3DScene* pScene, CMesh* pMesh, QString sText);

	//! Charge un fichier .mtl
	void loadMaterials(C3DScene* pScene, CMesh* pMesh, QString sText);

protected:

	COBJLoader();
};
