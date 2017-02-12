
#pragma once

#include "quick3d_global.h"

// Qt
#include <QThread>
#include <QMutex>

// Fondations
#include "CSingleton.h"

// Application
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

    //! Retire un travailleur du pool
    bool containsWorker(CWorker* pWorker);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Ajoute un travailleur au pool
    void addWorker(CWorker* pWorker);

    //! Retire un travailleur du pool
    void removeWorker(CWorker* pWorker);

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

protected:

    //! Conctructeur par d�faut
    CWorkerManager();

    //! Destructor
    virtual ~CWorkerManager();

    //! Impl�mentation de la m�thode run() de QThread
    virtual void run();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex              m_mMutex;			// Mutex de protection des acc�s concurrents
    QVector<CWorker*>   m_vWorkers;			// Pool de travailleurs
    bool                m_bStopRequested;	// Indique si un arr�t du travail est demand�
};
