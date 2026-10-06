
// Qt
#include <QDir>
#include <QFile>
#include <QTextStream>

// qt-plus
#include "CLogger.h"

// Application
#include "CRessourcesManager.h"
#include "CWaterMaterial.h"
#include "COBJLoader.h"
#include "CQ3DLoader.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Resolves \a sRelativePath inside \a sBaseDirectory, matching each path
    component case-insensitively. Returns the existing file path, or an empty
    string when nothing matches. \br\br
    Windows-authored assets reference folders like "Textures/" while the
    deployed tree uses "textures/" : exact lookup fails on case-sensitive
    file systems, so this fallback keeps those assets working on Linux.
*/
static QString locateFileCaseInsensitive(const QString& sBaseDirectory, const QString& sRelativePath)
{
    QDir tCurrent(sBaseDirectory);

    if (tCurrent.exists() == false)
    {
        return QString("");
    }

    QStringList lComponents = sRelativePath.split("/", Qt::SkipEmptyParts);

    for (int iIndex = 0; iIndex < lComponents.count(); iIndex++)
    {
        bool bLastComponent = (iIndex == lComponents.count() - 1);
        QString sWanted = lComponents[iIndex];

        if (sWanted == "." || sWanted == "..")
        {
            return QString("");
        }

        QStringList lEntries = tCurrent.entryList(
            bLastComponent ? (QDir::Files | QDir::NoDotAndDotDot) : (QDir::Dirs | QDir::NoDotAndDotDot));

        bool bFound = false;

        foreach (const QString& sEntry, lEntries)
        {
            if (sEntry.compare(sWanted, Qt::CaseInsensitive) == 0)
            {
                if (bLastComponent)
                {
                    QFileInfo tInfo(tCurrent.absoluteFilePath(sEntry));

                    if (tInfo.isFile() && tInfo.isReadable())
                    {
                        return tInfo.absoluteFilePath();
                    }

                    return QString("");
                }

                if (tCurrent.cd(sEntry) == false)
                {
                    return QString("");
                }

                bFound = true;
                break;
            }
        }

        if (bLastComponent == false && bFound == false)
        {
            return QString("");
        }
    }

    return QString("");
}

//-------------------------------------------------------------------------------------------------

CRessourcesManager::CRessourcesManager(C3DScene* pScene)
    : m_pScene(pScene)
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
    m_pTreeMaterial->diffuse() = CVector4(0.5, 0.3, 0.1, 1.0);
    m_pTreeMaterial->addDiffuseTexture(QString(""),"textures/Tree01.jpg");
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
        QString sFinalName = QString(":/resources/%1").arg(sFileToLocate);
        QFileInfo iFinalInfo(sFinalName);
        if (iFinalInfo.exists())
        {
            return sFinalName;
        }
    }

    // Case-insensitive fallback for Windows-authored paths ("Textures/..."
    // deployed as "textures/...") : exact lookup fails on Linux file systems
    if (sBaseFile.isEmpty() == false)
    {
        QFileInfo iBaseInfo(sBaseFile);
        QString sFallback = locateFileCaseInsensitive(iBaseInfo.absolutePath(), sFileToLocate);

        if (sFallback.isEmpty() == false)
        {
            LOG_DEBUG(QString("CRessourcesManager::locateResource() : %1 resolved as %2").arg(sFileToLocate, sFallback));

            return sFallback;
        }
    }

    {
        QString sFallback = locateFileCaseInsensitive(QCoreApplication::applicationDirPath(), sFileToLocate);

        if (sFallback.isEmpty() == false)
        {
            LOG_DEBUG(QString("CRessourcesManager::locateResource() : %1 resolved as %2").arg(sFileToLocate, sFallback));

            return sFallback;
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

    LOG_DEBUG(QString("CRessourcesManager::loadMesh() : %1 located as %2").arg(sMeshFileName, sFullFileName));

    QSP<CMeshGeometry> pLoadedMesh = findMesh(sFullFileName);

    if (pLoadedMesh != nullptr)
    {
        return pLoadedMesh;
    }

    if (sMeshFileName.contains(".obj"))
    {
        pLoadedMesh = COBJLoader::getInstance()->load(sBaseFile, pContainer, getObjByFilePathName(sFullFileName));

        LOG_DEBUG(QString("CRessourcesManager::loadMesh() : OBJ loaded = %1").arg(pLoadedMesh != nullptr ? "yes" : "null"));

        pLoadedMesh->setURL(sFullFileName);
    }
    else if (sMeshFileName.contains(".q3d"))
    {
        pLoadedMesh = CQ3DLoader::getInstance()->load(sBaseFile, pContainer, getObjByFilePathName(sFullFileName));

        LOG_DEBUG(QString("CRessourcesManager::loadMesh() : Q3D loaded = %1").arg(pLoadedMesh != nullptr ? "yes" : "null"));

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
