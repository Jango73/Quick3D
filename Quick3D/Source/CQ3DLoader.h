
#ifndef CQ3DLOADER_H
#define CQ3DLOADER_H

#include "quick3d_global.h"

// Application
#include "CSingleton.h"
#include "CMesh.h"
#include "CXMLNode.h"

class QUICK3D_EXPORT CQ3DLoader : public CSingleton<CQ3DLoader>
{
    friend class CSingleton<CQ3DLoader>;

public:

    //!
    void load(const QString& sBaseFile, C3DScene* pScene, CMesh* pMesh, QString sText);

    //!
    void loadComponent(const QString& sBaseFile, C3DScene* pScene, CMesh* pMesh, CXMLNode xNode, QVector<QSharedPointer<CMaterial> >& vMaterials, CComponent* pParent = NULL);

    //!
    void addBounds(CMesh* pMesh, CBoundingBox& bBox, Math::CMatrix4 mTransform);

protected:

    CQ3DLoader();
};

#endif // CQ3DLOADER_H
