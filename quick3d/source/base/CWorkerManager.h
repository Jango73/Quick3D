
#pragma once

// Qt
#include <QThread>
#include <QMutex>

// Std
#include <atomic>

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"
#include "CWorker.h"
#include "ISnapshotable.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWorkerManager : public QThread, public CSingleton<CWorkerManager>, public ISnapshotable
{
    Q_OBJECT

    friend class CSingleton<CWorkerManager>;

public:

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Removes a worker from the pool
    bool containsWorker(CWorker* pWorker);

    //! Tells whether any of \a vWorkers is in the pool, one lock for the whole batch
    bool containsAnyWorker(const QVector<CWorker*>& vWorkers);

    //! Returns the number of workers currently in the pool
    int pendingWorkerCount();

    //! Returns the pending worker count as a node, JSON through CXMLNode
    virtual CXMLNode snapshotNode() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Adds a worker to the pool
    void addWorker(CWorker* pWorker);

    //! Removes a worker from the pool
    void removeWorker(CWorker* pWorker);

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

protected:

    //! Default constructor
    CWorkerManager();

    //! Destructor
    virtual ~CWorkerManager();

    //! Implementation of the QThread run() method
    virtual void run();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QRecursiveMutex              m_mMutex;			// Mutex protecting concurrent accesses
    QVector<CWorker*>   m_vWorkers;			// Worker pool
    std::atomic<bool>   m_bStopRequested;	// Indicates whether stopping the work is requested
};
