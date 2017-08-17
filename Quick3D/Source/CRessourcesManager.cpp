
// Qt
#include <QFile>
#include <QTextStream>

//-------------------------------------------------------------------------------------------------

// Application
#include "CLogger.h"
#include "CRessourcesManager.h"
#include "CWaterMaterial.h"
#include "COBJLoader.h"
#include "CQ3DLoader.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CRessourcesManager::CRessourcesManager(C3DScene* pScene)
    : m_pScene(pScene)
    , m_mMutex(QMutex::Recursive)
    , m_pDefaultMaterial(nullptr)
    , m_pWaterMaterial(nullptr)
    , m_pSkyboxMaterial(nullptr)
    , m_pTreeMaterial(nullptr)
{
    m_pDefaultMaterial = QSP<CMaterial>(new CMaterial(pScene));

    m_pWaterMaterial = QSP<CMaterial>(new CWaterMaterial(pScene));

    m_pSkyboxMaterial = QSP<CMaterial>(new CMaterial(pScene));
    m_pSkyboxMaterial->setIRFactor(0.2);
    m_pSkyboxMaterial->setUseSky(true);

    m_pTreeMaterial = QSP<CMaterial>(new CMaterial(pScene));
    m_pTreeMaterial->diffuse() = Vector4(0.5, 0.3, 0.1, 1.0);
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
    m_vGeometry.clear();
    m_vMaterials.clear();
}

//-------------------------------------------------------------------------------------------------

void CRessourcesManager::clear()
{
    m_vMaterials.clear();
    m_vGeometry.clear();
}

//-------------------------------------------------------------------------------------------------

QString CRessourcesManager::locateResource(const QString& sBaseFile, const QString& sFileToLocate)
{
    // Check in file system, relative to base file
    if (sBaseFile.isEmpty() == false)
    {
        QFileInfo iBaseInfo(sBaseFile);
        QString sFinalName = iBaseInfo.absolutePath() + "/" + sFileToLocate;
        QFileInfo iFinalInfo(sFinalName);
        if (iFinalInfo.exists())
        {
            return sFinalName;
        }
    }

    // Check in file system, relative to executable
    {
        QString sFinalName = QCoreApplication::applicationDirPath() + "/" + sFileToLocate;
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

QSP<CMeshGeometry> CRessourcesManager::findMesh(const QString& sFullFileName)
{
    foreach (QSP<CMeshGeometry> pMesh, m_vGeometry)
    {
        if (pMesh->url() == sFullFileName)
        {
            return pMesh;
        }
    }

    return QSP<CMeshGeometry>(nullptr);
}

//-------------------------------------------------------------------------------------------------

QSP<CMeshGeometry> CRessourcesManager::loadMesh(const QString& sBaseFile, const QString& sMeshFileName, CComponent *pContainer)
{
    QString sFullFileName = locateResource(sBaseFile, sMeshFileName);

    QSP<CMeshGeometry> pLoadedMesh = findMesh(sFullFileName);

    if (pLoadedMesh)
    {
        return pLoadedMesh;
    }

    if (sMeshFileName.contains(".obj"))
    {
        pLoadedMesh = COBJLoader::getInstance()->load(sBaseFile, pContainer, getObjByFilePathName(sFullFileName));
        pLoadedMesh->setURL(sFullFileName);
    }
    else if (sMeshFileName.contains(".q3d"))
    {
        pLoadedMesh = CQ3DLoader::getInstance()->load(sBaseFile, pContainer, getObjByFilePathName(sFullFileName));
        pLoadedMesh->setURL(sFullFileName);
    }

    m_vGeometry.append(pLoadedMesh);

    return pLoadedMesh;
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

QSP<CMaterial> CRessourcesManager::shareMaterial(QSP<CMaterial> pMaterial)
{
    QMutexLocker locker(&m_mMutex);

    if (m_vMaterials.contains(pMaterial) == false)
    {
        m_vMaterials.append(pMaterial);
    }

    return pMaterial;
}
