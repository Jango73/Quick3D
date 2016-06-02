
// Qt
#include <QMutexLocker>

// Application
#include "Angles.h"
#include "CHGTData.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CHGTData::CHGTData(const Math::CGeoloc& gGeoloc, const Math::CGeoloc& gSize)
: m_tMutex(QMutex::Recursive)
, m_gGeoloc(gGeoloc)
, m_gSize(gSize)
, m_iNumCellsWidth(1201)
, m_iNumCellsHeight(1201)
, m_pFile(NULL)
{
}

//-------------------------------------------------------------------------------------------------

CHGTData::~CHGTData()
{
	if (m_pFile != NULL)
	{
		m_pFile->close();
		delete m_pFile;
	}
}

//-------------------------------------------------------------------------------------------------

double CHGTData::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
	QMutexLocker locker(&m_tMutex);

	if (pRigidness) *pRigidness = 1.0;

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
				short sData;
				int iOffset = ((((int) dLatPos + iLatOfs) * m_iNumCellsWidth) + ((int) dLonPos + iLonOfs)) * sizeof(short);
				if (m_pFile->seek(iOffset))
				{
					m_pFile->read((char*) &sData, sizeof(short));
					unsigned short* pShort = (unsigned short*) &sData;
					*pShort = ((*pShort & 0x00FF) << 8) | ((*pShort & 0xFF00) >> 8);

					if (iLatOfs == 0 && iLonOfs == 0) d1 = (double) sData;
					else if (iLatOfs == 0 && iLonOfs == 1) d2 = (double) sData;
					else if (iLatOfs == 1 && iLonOfs == 0) d3 = (double) sData;
					else if (iLatOfs == 1 && iLonOfs == 1) d4 = (double) sData;
				}
			}
		}
	}

	if (d1 == 0.0) d1 = -100.0;
	if (d2 == 0.0) d2 = -100.0;
	if (d3 == 0.0) d3 = -100.0;
	if (d4 == 0.0) d4 = -100.0;

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

//-------------------------------------------------------------------------------------------------

bool CHGTData::contains(const CGeoloc& gPosition) const
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

void CHGTData::setFileName(QString sFileName)
{
	m_sFileName = sFileName;

	if (m_pFile != NULL)
	{
		m_pFile->close();
		delete m_pFile;
	}

	m_pFile = new QFile(m_sFileName);
	m_pFile->open(QIODevice::ReadOnly);
}
