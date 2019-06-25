
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QHash>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

// Application
#include "CMeshInstance.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CRessourcesManager
{
public:

    //! Conctructeur
    CRessourcesManager(C3DScene* pScene);

    //! Destructor
    virtual ~CRessourcesManager();

    //!
    void clear();

    //!
    QString locateResource(const QString& sBaseFile, const QString& sFileToLocate);

    //!
    QSP<CMeshGeometry> findMesh(const QString& sFullFileName);

    //!
    QSP<CMeshGeometry> loadMesh(const QString& sBaseFile, const QString& sMeshFileName, CComponent* pContainer);

    //! Returns a shader by its name
    QString getShaderByFilePathName(const QString& filePathName);

    //! Returns an object file (.obj) by its name
    QString getObjByFilePathName(const QString& filePathName);

    //! Returns an icon by its name
    QIcon* getIconByFilePathName(const QString& filePathName);

    //! Returns a pixmap by its name
    QPixmap* getPixmapByFilePathName(const QString& filePathName);

    //!
    QSP<CMaterial> shareMaterial(QSP<CMaterial> pMaterial);

    //!
    QSP<CMaterial> getDefaultMaterial() { return m_pDefaultMaterial; }

    //!
    QSP<CMaterial> getWaterMaterial() { return m_pWaterMaterial; }

    //!
    QSP<CMaterial> getSkyboxMaterial() { return m_pSkyboxMaterial; }

    //!
    QSP<CMaterial> getTreeMaterial() { return m_pTreeMaterial; }

protected:

    QMutex      m_mMutex;
    C3DScene*   m_pScene;

    //! Hash table used to store shaders
    QHash<QString, QString> m_Shaders;

    //! Hash table used to store .obj files
    QHash<QString, QString> m_Objs;

    //! Hash table used to store icons
    QHash<QString, QIcon*> m_Icons;

    //! Mesh table
    QVector<QSP<CMeshGeometry> > m_vGeometry;

    //! Material table
    QVector<QSP<CMaterial> > m_vMaterials;

    //!
    QSP<CMaterial> m_pDefaultMaterial;

    //!
    QSP<CMaterial> m_pWaterMaterial;

    //!
    QSP<CMaterial> m_pSkyboxMaterial;

    //!
    QSP<CMaterial> m_pTreeMaterial;
};
