
// Qt
#include <QDir>
#include <QStringList>

// qt-plus
#include "CLogger.h"

// Application
#include "CBILField.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CBILField
    \brief A height field based on .BIL terrain files.
    \inmodule Quick3D
    \sa CHeightField
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CBILField using parameters in \a xParameters. \br\br
    \a sPathToBILFiles specifies the path where the .BIL files can be found. \br
    \a dValueForNoData if the special value that identifies empty data.
*/
CBILField::CBILField(CXMLNode xParameters, const QString& sPathToBILFiles, double dValueForNoData)
    : m_tMutex(QMutex::Recursive)
    , m_xParameters(xParameters)
    , m_dValueForNoData(dValueForNoData)
{
    LOG_METHOD_DEBUG(sPathToBILFiles);

    // Set path to argument path if not empty, else default to application path + "/BIL"

    if (sPathToBILFiles.isEmpty() == false)
    {
        m_sPath = sPathToBILFiles;
    }
    else
    {
        m_sPath = QCoreApplication::applicationDirPath() + "/BIL";
    }

    parseBILFiles();
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CBILField.
*/
CBILField::~CBILField()
{
    // Delete each chunk of data

    foreach (CBILData* pChunk, m_vChunks)
    {
        delete pChunk;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Parses all .BIL files in the given path.
*/
void CBILField::parseBILFiles()
{
    // List all BIL files in path

    QStringList nameFilter;

    nameFilter.append(QString("*.zip"));

    QDir dDirectory(m_sPath);
    QStringList lFiles = dDirectory.entryList(nameFilter);

    // Create a chunk for each file

    foreach (QString sFile, lFiles)
    {
        LOG_METHOD_DEBUG(QString("Creating chunk for %1...").arg(sFile));

        CBILData* pChunk = new CBILData(m_dValueForNoData);

        pChunk->setFileName(m_sPath + "/" + sFile);

        m_vChunks.append(pChunk);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    This is called by the getHeightAt() methods. Every 2000 calls, it deletes chunks which have not been used in the last 20 seconds.
*/
void CBILField::collectGarbage()
{
    QMutexLocker locker(&m_tMutex);

    static int iGarbageCounter = 0;

    iGarbageCounter++;

    if (iGarbageCounter > 2000)
    {
        iGarbageCounter = 0;

        for (int iIndex = 0; iIndex < m_vChunks.count(); iIndex++)
        {
            if (m_vChunks[iIndex]->isExpendable())
            {
                m_vChunks[iIndex]->clearData();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a pRigidness, if not nullptr, is filled with the terrain rigidness at the specified location. Always 1.0 for now.
*/
double CBILField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness != nullptr) *pRigidness = 1.0;

    foreach (CBILData* pChunk, m_vChunks)
    {
        if (pChunk->contains(gPosition))
        {
            return pChunk->getHeightAt(gPosition, pRigidness);
        }
    }

    collectGarbage();

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a pRigidness, if not nullptr, is filled with the terrain rigidness at the specified location. Always 1.0 for now.
    \a aAxis
*/
double CBILField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
    double dReturnValue = getHeightAt(CGeoloc(vPosition), pRigidness);

    collectGarbage();

    return dReturnValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a aAxis
    \a bForPhysics
*/
double CBILField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    double dReturnValue = getHeightAt(CGeoloc(vPosition), nullptr);

    collectGarbage();

    return dReturnValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Flattens terrain at the specified \a gPosition, to the extents of \a dRadius.
*/
void CBILField::flatten(const CGeoloc& gPosition, double dRadius)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds the \a pData chunk to this field.
*/
void CBILField::addChunk(CBILData* pData)
{
    if (m_vChunks.contains(pData) == false)
    {
        m_vChunks.append(pData);
    }
}
