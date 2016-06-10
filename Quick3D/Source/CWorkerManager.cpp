
// Qt
#include <QMutexLocker>

// Fondations
#include "CLogManager.h"

// Application
#include "CWorkerManager.h"

//-------------------------------------------------------------------------------------------------

#define MAX_WORKERS_RUNNING 3

//-------------------------------------------------------------------------------------------------

CWorkerManager::CWorkerManager()
    : m_mMutex(QMutex::Recursive)
    , m_bStopRequested(false)
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
    return m_vWorkers.contains(pWorker);
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
            if (pWorker->isStarted() == true && pWorker->isFinished() == false)
            {
                pWorker->abort();
            }

            m_vWorkers.remove(iIndex);
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

            for (int iIndex = 0; iIndex < MAX_WORKERS_RUNNING; iIndex++)
            {
                if (m_vWorkers.count() > iIndex)
                {
                    if (m_vWorkers[iIndex]->isStarted() == false)
                    {
                        m_vWorkers[iIndex]->start();
                        m_vWorkers[iIndex]->setPriority(QThread::HighPriority);
                    }
                    else
                    {
                        if (m_vWorkers[iIndex]->isFinished())
                        {
                            m_vWorkers.remove(iIndex);
                            iIndex--;
                        }
                    }
                }
            }
        }

        msleep(50);
    }
}
