
#pragma once

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"
#include "CMeshGeometry.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT COBJLoader : public CSingleton<COBJLoader>
{
    friend class CSingleton<COBJLoader>;

public:

    //! Charge un fichier .obj
    QSP<CMeshGeometry> load(const QString& sBaseFile, CComponent* pContainer, QString sText);

    //! Charge un fichier .mtl
    void loadMaterials(const QString& sBaseFile, C3DScene* pScene, CMeshGeometry* pMesh, QString sText);

protected:

    COBJLoader();
};
