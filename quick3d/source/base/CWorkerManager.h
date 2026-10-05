
#pragma once

// Qt
#include <QThread>
#include <QMutex>

// qt-plus
#include "CSingleton.h"

// Application
#include "quick3d_global.h"
#include "CWorker.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWorkerManager : public QThread, public CSingleton<CWorkerManager>
{
    Q_OBJECT

    friend class CSingleton<CWorkerManager>;

public:

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Removes a worker from the pool
    bool containsWorker(CWorker* pWorker);

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
    bool                m_bStopRequested;	// Indicates whether stopping the work is requested
};
