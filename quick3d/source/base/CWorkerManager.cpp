
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
    // Signal the service thread without holding the mutex: run() polls this
    // flag without the mutex, and joining the thread while holding m_mMutex
    // would deadlock (run() needs the mutex to service the queue).
    m_bStopRequested = true;
    wait();

    // Service thread exited: drain the pool one worker at a time. Each step
    // takes the mutex only to pick the next worker; removeWorker() joins it
    // outside any held lock because a finishing worker may need the
    // singleton pool lock (logging, getInstance) or this mutex to wind down.
    for (;;)
    {
        CWorker* pWorker = nullptr;

        {
            QMutexLocker locker(&m_mMutex);

            if (m_vWorkers.isEmpty())
            {
                break;
            }

            pWorker = m_vWorkers.first();
        }

        removeWorker(pWorker);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Tells whether any of \a vWorkers is in the pool.
*/
/***************************************************************************/
bool CWorkerManager::containsWorker(CWorker* pWorker)
{
    QMutexLocker locker(&m_mMutex);

    return m_vWorkers.contains(pWorker);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Tells whether any of \a vWorkers is in the pool, one lock for the whole batch.
*/
/***************************************************************************/
bool CWorkerManager::containsAnyWorker(const QVector<CWorker*>& vWorkers)
{
    QMutexLocker locker(&m_mMutex);

    foreach (CWorker* pWorker, vWorkers)
    {
        if (m_vWorkers.contains(pWorker) == true)
        {
            return true;
        }
    }

    return false;
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
    Returns the pending worker count as a node, JSON through CXMLNode.
*/
CXMLNode CWorkerManager::snapshotNode()
{
    CXMLNode xNode("workers");

    xNode.attributes()["class"] = "CWorkerManager";
    xNode.attributes()["pendingworkers"] = QString::number(pendingWorkerCount());

    return xNode;
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
    bool bFound = false;

    {
        QMutexLocker locker(&m_mMutex);

        for (int iIndex = 0; iIndex < m_vWorkers.count(); iIndex++)
        {
            if (m_vWorkers[iIndex] == pWorker)
            {
                m_vWorkers.remove(iIndex);
                bFound = true;
                break;
            }
        }
    }

    if (bFound == true)
    {
        // Wait outside any lock: a worker whose finished flag is
        // already set while its thread is still exiting must be joined
        // before its owner destroys it. wait() on a finished thread
        // returns immediately. Joining under m_mMutex would deadlock a
        // finishing worker that needs the mutex or the singleton pool.
        if (pWorker->isStarted() == true)
        {
            pWorker->abort();
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
