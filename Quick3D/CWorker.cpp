
// Fondations
#include "CLogManager.h"

// Application
#include "CWorker.h"

//-------------------------------------------------------------------------------------------------

CWorker::CWorker()
: m_bStopRequested(false)
, m_bStarted(false)
, m_bFinished(false)
{
}

//-------------------------------------------------------------------------------------------------

CWorker::~CWorker()
{
}

//-------------------------------------------------------------------------------------------------

void CWorker::work()
{
}

//-------------------------------------------------------------------------------------------------

void CWorker::abort()
{
	m_bStopRequested = true;
	wait();
	m_bStopRequested = false;
}

//-------------------------------------------------------------------------------------------------

void CWorker::doSleep(int ms)
{
}

//-------------------------------------------------------------------------------------------------

void CWorker::run()
{
	LOG_DEBUG(QString("CWorker::run() : START : Thread ID = %1").arg((qlonglong) QThread::currentThreadId()));

	m_bStarted = true;

	work();

	m_bFinished = true;

	LOG_DEBUG(QString("CWorker::run() : FINISHED : Thread ID = %1").arg((qlonglong) QThread::currentThreadId()));
}
