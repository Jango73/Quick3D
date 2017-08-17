
// Qt
#include <QDir>
#include <QStringList>

// Application
#include "CSRTMField.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CSRTMField::CSRTMField(CXMLNode xParameters, const QString& sPathToSRTMFiles, double dValueForNoData)
    : m_xParameters(xParameters)
    , m_dValueForNoData(dValueForNoData)
{
    if (sPathToSRTMFiles.isEmpty() == false)
    {
        // Compile parameters
        m_sPath = sPathToSRTMFiles;
    }
    else
    {
        // Compile parameters
        m_sPath = QCoreApplication::applicationDirPath() + "/SRTM";
    }

    parseSRTMFiles();
}

//-------------------------------------------------------------------------------------------------

CSRTMField::~CSRTMField()
{
    foreach (CSRTMData* pChunk, m_vChunks)
    {
        delete pChunk;
    }
}

//-------------------------------------------------------------------------------------------------

void CSRTMField::parseSRTMFiles()
{
    QStringList nameFilter;

    nameFilter.append(QString("*.asc"));
    nameFilter.append(QString("*.bin"));

    QDir dDirectory(m_sPath);
    QStringList lFiles = dDirectory.entryList(nameFilter);

    foreach (QString sFile, lFiles)
    {
        CSRTMData* pChunk = new CSRTMData(m_dValueForNoData);

        pChunk->setFileName(m_sPath + "/" + sFile);

        m_vChunks.append(pChunk);
    }
}

//-------------------------------------------------------------------------------------------------

double CSRTMField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness) *pRigidness = 1.0;

    foreach (CSRTMData* pChunk, m_vChunks)
    {
        if (pChunk->contains(gPosition))
        {
            return pChunk->getHeightAt(gPosition, pRigidness);
        }
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CSRTMField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
    return getHeightAt(CGeoloc(vPosition), pRigidness);
}

//-------------------------------------------------------------------------------------------------

double CSRTMField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    return getHeightAt(CGeoloc(vPosition), nullptr);
}

//-------------------------------------------------------------------------------------------------

void CSRTMField::flatten(const CGeoloc& gPosition, double dRadius)
{
}

//-------------------------------------------------------------------------------------------------

void CSRTMField::addChunk(CSRTMData* pData)
{
    m_vChunks.append(pData);
}
