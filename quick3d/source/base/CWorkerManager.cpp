
// Qt
#include <QMutexLocker>

// qt-plus
#include "CLogger.h"

// Application
#include "CWorkerManager.h"

//-------------------------------------------------------------------------------------------------

#define MAX_WORKERS_RUNNING 3

//-------------------------------------------------------------------------------------------------

CWorkerManager::CWorkerManager()
    : m_bStopRequested(false)
{
    start();
}

//-------------------------------------------------------------------------------------------------

CWorkerManager::~CWorkerManager()
{
    QMutexLocker locker(&m_mMutex);

    m_bStopRequested = true;
    wait();

    foreach (CWorker* pWorker, m_vWorkers)
    {
        if (pWorker->isStarted())
        {
            pWorker->abort();
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool CWorkerManager::containsWorker(CWorker* pWorker)
{
    QMutexLocker locker(&m_mMutex);

    return m_vWorkers.contains(pWorker);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the number of workers currently in the pool.
*/
int CWorkerManager::pendingWorkerCount()
{
    QMutexLocker locker(&m_mMutex);

    return m_vWorkers.count();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a JSON document with the pending worker count.
*/
QString CWorkerManager::snapshotJson()
{
    return QString("{\"class\":\"CWorkerManager\",\"pendingworkers\":%1}").arg(pendingWorkerCount());
}

//-------------------------------------------------------------------------------------------------

void CWorkerManager::addWorker(CWorker* pWorker)
{
    QMutexLocker locker(&m_mMutex);

    if (m_vWorkers.contains(pWorker) == false)
    {
        m_vWorkers.append(pWorker);
    }
}

//-------------------------------------------------------------------------------------------------

void CWorkerManager::removeWorker(CWorker* pWorker)
{
    QMutexLocker locker(&m_mMutex);

    for (int iIndex = 0; iIndex < m_vWorkers.count(); iIndex++)
    {
        if (m_vWorkers[iIndex] == pWorker)
        {
            m_vWorkers.remove(iIndex);

            // Wait outside any ambiguity: a worker whose finished flag is
            // already set while its thread is still exiting must be joined
            // before its owner destroys it. wait() on a finished thread
            // returns immediately.
            if (pWorker->isStarted() == true)
            {
                pWorker->abort();
            }

            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorkerManager::run()
{
    while (m_bStopRequested == false)
    {
        {
            QMutexLocker locker(&m_mMutex);

            // Service the whole queue: count the running workers instead of
            // pinning service to the first slots, so a blocked head worker
            // can never starve the workers queued behind it.
            // Finished workers are reaped at any index.
            int iRunningWorkers = 0;

            for (int iIndex = 0; iIndex < m_vWorkers.count(); iIndex++)
            {
                if (m_vWorkers[iIndex]->isFinished())
                {
                    m_vWorkers.remove(iIndex);
                    iIndex--;
                }
                else if (m_vWorkers[iIndex]->isStarted() == false)
                {
                    if (iRunningWorkers < MAX_WORKERS_RUNNING)
                    {
                        m_vWorkers[iIndex]->start();
                        m_vWorkers[iIndex]->setPriority(QThread::HighPriority);
                        iRunningWorkers++;
                    }
                }
                else
                {
                    iRunningWorkers++;
                }
            }
        }

        msleep(50);
    }
}
