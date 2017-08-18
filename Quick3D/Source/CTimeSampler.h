
#pragma once

// Qt
#include <QObject>
#include <QDateTime>
#include <QTimer>

// qt-plus
#include "CSingleton.h"

// Fondations
#include "quick3d_global.h"
#include "CTracableMutex.h"

//-------------------------------------------------------------------------------------------------

#define START_SAMPLE(a)         CTimeSampler::getInstance()->startSample(a)
#define STOP_SAMPLE(a)          CTimeSampler::getInstance()->stopSample(a)
#define STOP_SAMPLE_AND_LOG(a)  CTimeSampler::getInstance()->stopSampleAndLog(a)

class CLogger;

class QUICK3D_EXPORT CTimeSampler : public QObject, public CSingleton<CTimeSampler>
{
    Q_OBJECT

public:

    //! Default constructor
    CTimeSampler();

    CTimeSampler(CLogger* pLogger);

    //! Destructor
    virtual ~CTimeSampler();

    //! Starts sampling
    void startSample(const QString& sName);

    //! Stops sampling
    void stopSample(const QString& sName);

    //! Stops sampling and log results
    void stopSampleAndLog(const QString& sName);

    void dumpAccumulatedTimes();

private slots:

    void onTimer();

private:

    CTracableMutex              m_tMutex;
    QMap<QString, QDateTime>    m_mStartTimes;
    QMap<QString, qint64>       m_mAccumTimes;
    QMap<QString, qint64>       m_mNumCalls;
    QTimer                      m_tDumpTimer;
    CLogger*                    m_pLogger;
};
