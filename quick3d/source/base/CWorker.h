
#pragma once

// Qt
#include <QThread>

// Std
#include <atomic>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWorker : public QThread
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CWorker();

    //! Destructor
    virtual ~CWorker();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns true if the work has started
    bool isStarted() { return m_bStarted; }

    //! Returns true if the work is finished
    bool isFinished() { return m_bFinished; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Work method
    virtual void work();

    //! Cancels the work of the object
    void abort();

    //! Resets the worker so it can be queued again after a run
    void resetForReuse();

    //! Sleeps
    void doSleep(int ms);

    //-------------------------------------------------------------------------------------------------
    // Protected inherited methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Implementation of the QThread run() method
    virtual void run();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    std::atomic<bool>   m_bStopRequested;   // Indicates whether stopping the work is requested

private:

    std::atomic<bool>   m_bStarted;         // Indicates whether the work has started
    std::atomic<bool>   m_bFinished;        // Indicates whether the work is finished
};
