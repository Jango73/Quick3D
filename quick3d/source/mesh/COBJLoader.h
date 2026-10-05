
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

    //! Loads an .obj file
    QSP<CMeshGeometry> load(const QString& sBaseFile, CComponent* pContainer, QString sText);

    //! Loads an .mtl file
    void loadMaterials(const QString& sBaseFile, C3DScene* pScene, CMeshGeometry* pMesh, QString sText);

protected:

    COBJLoader();
};
