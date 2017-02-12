
#ifndef CTIMESAMPLER_H
#define CTIMESAMPLER_H

#include "quick3d_global.h"

// QT
#include <QObject>
#include <QDateTime>
#include <QTimer>

// Fondations
#include "CSingleton.h"
#include "CTracableMutex.h"

#define START_SAMPLE(a)			CTimeSampler::getInstance()->startSample(a)
#define STOP_SAMPLE(a)			CTimeSampler::getInstance()->stopSample(a)
#define STOP_SAMPLE_AND_LOG(a)	CTimeSampler::getInstance()->stopSampleAndLog(a)

class CLogManager;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTimeSampler : public QObject, public CSingleton<CTimeSampler>
{
	Q_OBJECT

public:

	//! Default constructor
	CTimeSampler();

	CTimeSampler(CLogManager* pLogger);

	//! Destructor
	virtual ~CTimeSampler();

	void startSample(const QString& sName);

	void stopSample(const QString& sName);

	void stopSampleAndLog(const QString& sName);

	void dumpAccumulatedTimes();

private slots:

	void onTimer();

private:

	CTracableMutex				m_tMutex;
	QMap<QString, QDateTime>	m_mStartTimes;
	QMap<QString, qint64>		m_mAccumTimes;
	QMap<QString, qint64>		m_mNumCalls;
	QTimer						m_tDumpTimer;
	CLogManager*				m_pLogger;
};

#endif // CTIMESAMPLER_H
