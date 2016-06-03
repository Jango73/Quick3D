
// Qt
#include <QDir>
#include <QStringList>

// Application
#include "CHGTField.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CHGTField::CHGTField(CXMLNode xParameters, const QString& sPathToHGTFiles)
    : m_xParameters(xParameters)
{
    if (sPathToHGTFiles.isEmpty() == false)
    {
        m_sPath = sPathToHGTFiles;
    }
    else
    {
        m_sPath = QCoreApplication::applicationDirPath() + "/HGT";
    }

    parseFiles();
}

//-------------------------------------------------------------------------------------------------

CHGTField::~CHGTField()
{
    foreach (CHGTData* pChunk, m_vChunks)
    {
        delete pChunk;
    }
}

//-------------------------------------------------------------------------------------------------

void CHGTField::parseFiles()
{
    QStringList nameFilter("*.hgt");
    QDir dDirectory(m_sPath);
    QStringList lFiles = dDirectory.entryList(nameFilter);

    foreach (QString sFile, lFiles)
    {
        QString sLatSign = sFile.mid(0, 1);
        QString sLat = sFile.mid(1, 2);
        QString sLonSign = sFile.mid(3, 1);
        QString sLon = sFile.mid(4, 3);

        double dLat = sLat.toDouble() * (sLatSign == "N" ? 1.0 : -1.0);
        double dLon = sLon.toDouble() * (sLonSign == "E" ? 1.0 : -1.0);

        dLat = Angles::clipAngleDegree(dLat);
        dLon = Angles::clipAngleDegree(dLon);

        CGeoloc gGeoloc(dLat, dLon, 0.0);
        CGeoloc gSize(1.0, 1.0, 0.0);

        CHGTData* pChunk = new CHGTData(gGeoloc, gSize);

        pChunk->setFileName(m_sPath + "/" + sFile);

        m_vChunks.append(pChunk);
    }
}

//-------------------------------------------------------------------------------------------------

double CHGTField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness) *pRigidness = 1.0;

    foreach (CHGTData* pChunk, m_vChunks)
    {
        if (pChunk->contains(gPosition))
        {
            return pChunk->getHeightAt(gPosition, pRigidness);
        }
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CHGTField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
    return getHeightAt(CGeoloc(vPosition), pRigidness);
}

//-------------------------------------------------------------------------------------------------

double CHGTField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    double dReturnValue = getHeightAt(CGeoloc(vPosition), NULL);

    if (!bForPhysics)
    {
        if (dReturnValue < 0.0) dReturnValue = 0.0;
    }

    return dReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CHGTField::flatten(const Math::CGeoloc& gPosition, double dRadius)
{
}
