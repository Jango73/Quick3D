
// Qt
#include <QFile>
#include <QTextStream>

//-------------------------------------------------------------------------------------------------

// Application
#include "CLogManager.h"
#include "CRessourcesManager.h"
#include "CWaterMaterial.h"
#include "COBJLoader.h"
#include "CQ3DLoader.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CRessourcesManager::CRessourcesManager(C3DScene* pScene)
    : m_pScene(pScene)
    , m_mMutex(QMutex::Recursive)
    , m_pDefaultMaterial(NULL)
    , m_pWaterMaterial(NULL)
    , m_pSkyboxMaterial(NULL)
    , m_pTreeMaterial(NULL)
{
    m_pDefaultMaterial = QSharedPointer<CMaterial>(new CMaterial(pScene));

    m_pWaterMaterial = QSharedPointer<CMaterial>(new CWaterMaterial(pScene));

    m_pSkyboxMaterial = QSharedPointer<CMaterial>(new CMaterial(pScene));
    m_pSkyboxMaterial->setIRFactor(0.2);
    m_pSkyboxMaterial->setUseSky(true);

    m_pTreeMaterial = QSharedPointer<CMaterial>(new CMaterial(pScene));
    m_pTreeMaterial->getDiffuse() = Vector4(0.5, 0.3, 0.1, 1.0);
    m_pTreeMaterial->addDiffuseTexture(QString(""),"Textures/Tree01.jpg");
    m_pTreeMaterial->setIRFactor(0.2);
}

//-------------------------------------------------------------------------------------------------

CRessourcesManager::~CRessourcesManager()
{
    foreach (QIcon* icon, m_Icons)
    {
        delete icon;
    }

    m_Icons.clear();

    foreach (QPixmap* pixmap, m_Pixmaps)
    {
        delete pixmap;
    }

    m_Pixmaps.clear();

    m_vMeshes.clear();

    m_vMaterials.clear();
}

//-------------------------------------------------------------------------------------------------

QString CRessourcesManager::locateResource(const QString& sBaseFile, const QString& sFileToLocate)
{
    // Check in file system, relative to base file
    if (sBaseFile.isEmpty() == false)
    {
        QFileInfo iBaseInfo(sBaseFile);
        QString sFinalName = iBaseInfo.absolutePath() + sFileToLocate;
        QFileInfo iFinalInfo(sFinalName);
        if (iFinalInfo.exists())
        {
            return sFinalName;
        }
    }

    // Check in resources
    {
        QString sFinalName = QString(":/Resources/%1").arg(sFileToLocate);
        QFileInfo iFinalInfo(sFinalName);
        if (iFinalInfo.exists())
        {
            return sFinalName;
        }
    }

    return QString("");
}

//-------------------------------------------------------------------------------------------------

void CRessourcesManager::loadMesh(const QString& sBaseFile, CMesh* pMesh, const QString& sMeshFileName)
{
    QString sFullFileName = locateResource(sBaseFile, sMeshFileName);

    if (sMeshFileName.contains(".obj"))
    {
        COBJLoader::getInstance()->load(sBaseFile, m_pScene, pMesh, getObjByFilePathName(sFullFileName));
    }
    else if (sMeshFileName.contains(".q3d"))
    {
        CQ3DLoader::getInstance()->load(sBaseFile, m_pScene, pMesh, getObjByFilePathName(sFullFileName));
    }
}

//-------------------------------------------------------------------------------------------------

QString CRessourcesManager::getShaderByFilePathName(const QString& filePathName)
{
    QHash<QString, QString>::iterator i = m_Shaders.find(filePathName);

    if (i == m_Shaders.constEnd())
    {
        QFile inputFile(filePathName);
        inputFile.open(QIODevice::ReadOnly);
        QTextStream in(&inputFile);
        QString sText = in.readAll();

        if (sText == "")
        {
            LOG_WARNING(QString("CRessourcesManager::getShaderByFilePathName() : shader %1 is empty").arg(filePathName));
        }

        inputFile.close();

        m_Shaders[filePathName] = sText;
    }

    return m_Shaders[filePathName];
}

//-------------------------------------------------------------------------------------------------

QString CRessourcesManager::getObjByFilePathName(const QString& filePathName)
{
    QHash<QString, QString>::iterator i = m_Objs.find(filePathName);

    if (i == m_Objs.constEnd())
    {
        QFile inputFile(filePathName);
        inputFile.open(QIODevice::ReadOnly);
        QTextStream in(&inputFile);
        QString sText = in.readAll();
        inputFile.close();

        m_Objs[filePathName] = sText;
    }

    return m_Objs[filePathName];
}

//-------------------------------------------------------------------------------------------------

QIcon* CRessourcesManager::getIconByFilePathName(const QString& filePathName)
{
    QHash<QString, QIcon*>::iterator i = m_Icons.find(filePathName);

    if (i == m_Icons.constEnd())
    {
        m_Icons[filePathName] = new QIcon(filePathName);
    }

    return m_Icons[filePathName];
}

//-------------------------------------------------------------------------------------------------

QPixmap* CRessourcesManager::getPixmapByFilePathName(const QString& filePathName)
{
    QHash<QString, QPixmap*>::iterator i = m_Pixmaps.find(filePathName);

    if (i == m_Pixmaps.constEnd())
    {
        m_Pixmaps[filePathName] = new QPixmap(filePathName);
    }

    return m_Pixmaps[filePathName];
}

//-------------------------------------------------------------------------------------------------

CMeshInstance* CRessourcesManager::share(QSharedPointer<CMesh> pMesh)
{
    QMutexLocker locker(&m_mMutex);

    if (m_vMeshes.contains(pMesh) == false)
    {
        m_vMeshes.append(pMesh);
    }

    return new CMeshInstance(pMesh);
}

//-------------------------------------------------------------------------------------------------

CMeshInstance* CRessourcesManager::share(QVector<QSharedPointer<CMesh> > vMeshes)
{
    QMutexLocker locker(&m_mMutex);

    foreach (QSharedPointer<CMesh> pMesh, vMeshes)
    {
        if (m_vMeshes.contains(pMesh) == false)
        {
            m_vMeshes.append(pMesh);
        }
    }

    return new CMeshInstance(vMeshes);
}

//-------------------------------------------------------------------------------------------------

QSharedPointer<CMaterial> CRessourcesManager::shareMaterial(QSharedPointer<CMaterial> pMaterial)
{
    QMutexLocker locker(&m_mMutex);

    if (m_vMaterials.contains(pMaterial) == false)
    {
        m_vMaterials.append(pMaterial);
    }

    return pMaterial;
}
