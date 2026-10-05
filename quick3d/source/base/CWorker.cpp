
// Fondations
#include "CLogger.h"

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

/*!
    Resets this worker so it can be queued again after a run. \br\br
    Waits for the thread to exit first: a worker whose flags read finished
    while its thread is still exiting must not be restarted too early.
*/
void CWorker::resetForReuse()
{
    wait();
    m_bStarted = false;
    m_bFinished = false;
}

//-------------------------------------------------------------------------------------------------

void CWorker::doSleep(int ms)
{
}

//-------------------------------------------------------------------------------------------------

void CWorker::run()
{
    LOG_METHOD_DEBUG(QString("START : Thread ID = %1").arg((qlonglong) QThread::currentThreadId()));

    m_bStarted = true;
    m_bFinished = false;

    work();

    m_bFinished = true;

    LOG_METHOD_DEBUG(QString("FINISHED : Thread ID = %1").arg((qlonglong) QThread::currentThreadId()));
}
