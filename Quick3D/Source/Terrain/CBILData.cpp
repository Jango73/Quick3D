
// Qt
#include <QFile>
#include <QTextStream>
#include <QDataStream>

// qt-plus
#include "CLogger.h"

// Application
#include "Angles.h"
#include "CZip.h"
#include "CBILData.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define NUM_HEADER_FIELDS	14

//-------------------------------------------------------------------------------------------------

/*
//! Byte swap unsigned short
quint16 swap_quint16( quint16 val ) 
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
qint16 swap_qint16( qint16 val )
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap int
qint32 swap_qint32( qint32 val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

qint64 swap_int64( qint64 val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

double swap_double( double dbl )
{
    qint64 val = *((qint64*) &dbl);
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    val = (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);

    return *((double*) &val);
}
*/

//-------------------------------------------------------------------------------------------------

CBILData::CBILData(double dValueForNoData)
    : CExpendable(20)
    , m_tMutex(QMutex::Recursive)
    , m_dValueForNoData(dValueForNoData)
    , m_iNumCellsWidth(0)
    , m_iNumCellsHeight(0)
    , m_ui16NoDataValue(0)
    , m_vData(nullptr)
{
    setUsedNow();
}

//-------------------------------------------------------------------------------------------------

CBILData::CBILData(
        CGeoloc gGeoloc,
        CGeoloc gSize,
        int iNumCellsWidth,
        int iNumCellsHeight,
        qint16 ui16NoDataValue,
        const qint16* vData,
        double dValueForNoData
        )
    : CExpendable(20)
    , m_tMutex(QMutex::Recursive)
    , m_dValueForNoData(dValueForNoData)
    , m_vData(nullptr)
{
    setUsedNow();

    if (iNumCellsWidth > 0 && iNumCellsHeight > 0 && vData != nullptr)
    {
        m_gGeoloc			= gGeoloc;
        m_gSize				= gSize;
        m_iNumCellsWidth	= iNumCellsWidth;
        m_iNumCellsHeight	= iNumCellsHeight;
        m_ui16NoDataValue	= ui16NoDataValue;
        m_vData				= new qint16[iNumCellsWidth * iNumCellsHeight];
        memcpy(m_vData, vData, (iNumCellsWidth * iNumCellsHeight) * sizeof(qint16));
    }
}

//-------------------------------------------------------------------------------------------------

CBILData::CBILData(const CBILData& target)
    : CExpendable(20)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CBILData::~CBILData()
{
    if (m_vData != nullptr)
    {
        delete m_vData;
    }
}

//-------------------------------------------------------------------------------------------------

CBILData& CBILData::operator = (const CBILData& target)
{
    m_gGeoloc			= target.m_gGeoloc;
    m_gSize				= target.m_gSize;
    m_iNumCellsWidth	= target.m_iNumCellsWidth;
    m_iNumCellsHeight	= target.m_iNumCellsHeight;
    m_ui16NoDataValue	= target.m_ui16NoDataValue;
    m_dValueForNoData	= target.m_dValueForNoData;
    m_vData				= nullptr;

    if (target.m_iNumCellsWidth > 0 && target.m_iNumCellsHeight > 0 && target.m_vData != nullptr)
    {
        m_vData = new qint16[target.m_iNumCellsWidth * target.m_iNumCellsHeight];
        memcpy(m_vData, target.m_vData, (target.m_iNumCellsWidth * target.m_iNumCellsHeight) * sizeof(qint16));
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------

double CBILData::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    QMutexLocker locker(&m_tMutex);

    setUsedNow();

    if (pRigidness != nullptr)
    {
        *pRigidness = 1.0;
    }

    if (m_vData == nullptr)
    {
        readData();
    }

    if (m_vData != nullptr)
    {
        double dLatDiff = gPosition.Latitude - m_gGeoloc.Latitude;
        double dLonDiff = gPosition.Longitude - m_gGeoloc.Longitude;

        dLatDiff = Angles::clipAngleDegree(dLatDiff);
        dLonDiff = Angles::clipAngleDegree(dLonDiff);

        double dLatPos = (1.0 - (dLatDiff / m_gSize.Latitude)) * ((double) m_iNumCellsHeight - 1);
        double dLonPos = (dLonDiff / m_gSize.Longitude) * ((double) m_iNumCellsWidth - 1);

        double d1 = 0.0;
        double d2 = 0.0;
        double d3 = 0.0;
        double d4 = 0.0;

        for (int iLatOfs = 0; iLatOfs < 2; iLatOfs++)
        {
            for (int iLonOfs = 0; iLonOfs < 2; iLonOfs++)
            {
                if (
                        ((int) dLatPos + iLatOfs) >= 0 && ((int) dLatPos + iLatOfs) < (m_iNumCellsHeight) &&
                        ((int) dLonPos + iLonOfs) >= 0 && ((int) dLonPos + iLonOfs) < (m_iNumCellsWidth)
                        )
                {
                    int iOffset = (((int) dLatPos + iLatOfs) * m_iNumCellsWidth) + ((int) dLonPos + iLonOfs);

                    if (iLatOfs == 0 && iLonOfs == 0) d1 = (double) m_vData[iOffset];
                    else if (iLatOfs == 0 && iLonOfs == 1) d2 = (double) m_vData[iOffset];
                    else if (iLatOfs == 1 && iLonOfs == 0) d3 = (double) m_vData[iOffset];
                    else if (iLatOfs == 1 && iLonOfs == 1) d4 = (double) m_vData[iOffset];
                }
            }
        }

        if (d1 == 0.0)
        {
            d1 = -100.0;
        }
        if (d2 == 0.0)
        {
            d2 = -100.0;
        }
        if (d3 == 0.0)
        {
            d3 = -100.0;
        }
        if (d4 == 0.0)
        {
            d4 = -100.0;
        }

        double fx = dLonPos - ((double) (int) dLonPos);
        double fy = dLatPos - ((double) (int) dLatPos);
        double fx1 = 1.0 - fx;
        double fy1 = 1.0 - fy;

        double w1 = fx1 * fy1;
        double w2 = fx  * fy1;
        double w3 = fx1 * fy;
        double w4 = fx  * fy;

        return d1 * w1 + d2 * w2 + d3 * w3 + d4 * w4;
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

bool CBILData::contains(const CGeoloc& gPosition) const
{
    double dLatDiff1 = Math::Angles::angleDifferenceDegree(m_gGeoloc.Latitude, gPosition.Latitude);
    double dLatDiff2 = Math::Angles::angleDifferenceDegree(gPosition.Latitude, m_gGeoloc.Latitude + m_gSize.Latitude);

    double dLonDiff1 = Math::Angles::angleDifferenceDegree(m_gGeoloc.Longitude, gPosition.Longitude);
    double dLonDiff2 = Math::Angles::angleDifferenceDegree(gPosition.Longitude, m_gGeoloc.Longitude + m_gSize.Longitude);

    if (dLatDiff1 <= 0.0 && dLatDiff2 <= 0.0 && dLonDiff1 <= 0.0 && dLonDiff2 <= 0.0)
    {
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void CBILData::setFileName(QString sFileName)
{
    LOG_METHOD_DEBUG(sFileName);

    m_sFileName = sFileName;

    if (m_vData != nullptr)
    {
        delete m_vData;
    }

    m_vData = nullptr;

    QStringList lHeader = readHeader();

    if (lHeader.count() == NUM_HEADER_FIELDS)
    {
        m_iNumCellsWidth = lHeader[2].toInt();
        m_iNumCellsHeight = lHeader[3].toInt();
        m_gGeoloc = CGeoloc(lHeader[10].toDouble() - 1.0, lHeader[9].toDouble(), 0.0);
        m_gSize = CGeoloc(1.0, 1.0, 0.0);
        m_ui16NoDataValue = (qint16) lHeader[13].toShort();
    }
}

//-------------------------------------------------------------------------------------------------

QStringList CBILData::readHeader()
{
    LOG_METHOD_DEBUG(m_sFileName);

    QStringList lReturnValue;

    CZip zFile(m_sFileName);
    QStringList slFiles = zFile.getZipFileList();

    foreach (QString sFile, slFiles)
    {
        if (sFile.contains(".hdr"))
        {
            QByteArray baContent = zFile.getZipFileContent(sFile);

            QTextStream in(&baContent);

            for (int iHeaderIndex = 0; iHeaderIndex < NUM_HEADER_FIELDS; iHeaderIndex++)
            {
                QStringList sList = in.readLine().split(" ", QString::SkipEmptyParts);

                if (sList.count() > 1)
                {
                    lReturnValue.append(sList[1]);
                }
            }
        }
        else if (sFile.contains(".bil"))
        {
            m_sBILFileName = sFile;
        }
    }

    return lReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CBILData::readData()
{
    LOG_METHOD_DEBUG(m_sFileName);

    m_vData = new qint16[m_iNumCellsWidth * m_iNumCellsHeight];

    if (m_sBILFileName.isEmpty() == false)
    {
        CZip zFile(m_sFileName);
        QByteArray baContent = zFile.getZipFileContent(m_sBILFileName);
        QDataStream sStream(baContent);

        int iDataSize = (m_iNumCellsWidth * m_iNumCellsHeight) * sizeof(qint16);

        sStream.readRawData((char*) m_vData, iDataSize);

        // Ajustements

        for (int iIndex = 0; iIndex < (m_iNumCellsWidth * m_iNumCellsHeight); iIndex++)
        {
            if (m_vData[iIndex] == m_ui16NoDataValue)
            {
                m_vData[iIndex] = (int) m_dValueForNoData;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CBILData::clearData()
{
    if (m_vData != nullptr)
    {
        delete [] m_vData;
        m_vData = nullptr;
    }
}
