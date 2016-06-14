
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CSingleton.h"

// Application
#include "CMeshGeometry.h"

class QUICK3D_EXPORT COBJLoader : public CSingleton<COBJLoader>
{
    friend class CSingleton<COBJLoader>;

public:

    //! Charge un fichier .obj
    QSharedPointer<CMeshGeometry> load(const QString& sBaseFile, CComponent* pContainer, QString sText);

    //! Charge un fichier .mtl
    void loadMaterials(const QString& sBaseFile, C3DScene* pScene, CMeshGeometry* pMesh, QString sText);

protected:

    COBJLoader();
};
