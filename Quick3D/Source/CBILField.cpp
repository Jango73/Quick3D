
// Qt
#include <QDir>
#include <QStringList>

// Fondations
#include "CLogManager.h"

// Application
#include "CBILField.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBILField::CBILField(CXMLNode xParameters, const QString& sPathToBILFiles, double dValueForNoData)
: m_tMutex(QMutex::Recursive)
, m_xParameters(xParameters)
, m_dValueForNoData(dValueForNoData)
{
	LOG_DEBUG(QString("CBILField::CBILField(%1)").arg(sPathToBILFiles));

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

CBILField::~CBILField()
{
	foreach (CBILData* pChunk, m_vChunks)
	{
		delete pChunk;
	}
}

//-------------------------------------------------------------------------------------------------

void CBILField::parseBILFiles()
{
	LOG_DEBUG(QString("CBILField::parseBILFiles()"));

	QStringList nameFilter;

	nameFilter.append(QString("*.zip"));

	QDir dDirectory(m_sPath);
	QStringList lFiles = dDirectory.entryList(nameFilter);

	foreach (QString sFile, lFiles)
	{
		LOG_DEBUG(QString("CBILField::parseBILFiles() : Creating chunk for %1...").arg(sFile));

		CBILData* pChunk = new CBILData(m_dValueForNoData);

		pChunk->setFileName(m_sPath + "/" + sFile);

		m_vChunks.append(pChunk);
	}
}

//-------------------------------------------------------------------------------------------------

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
			if (m_vChunks[iIndex]->getLastUsed().secsTo(QDateTime::currentDateTime()) > 20)
			{
				m_vChunks[iIndex]->clearData();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

double CBILField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
	if (pRigidness) *pRigidness = 1.0;

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

double CBILField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
	double dReturnValue = getHeightAt(CGeoloc(vPosition), pRigidness);

	collectGarbage();

	return dReturnValue;
}

//-------------------------------------------------------------------------------------------------

double CBILField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
	double dReturnValue = getHeightAt(CGeoloc(vPosition), NULL);

	collectGarbage();

	return dReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CBILField::flatten(const Math::CGeoloc& gPosition, double dRadius)
{
}

//-------------------------------------------------------------------------------------------------

void CBILField::addChunk(CBILData* pData)
{
	m_vChunks.append(pData);
}
