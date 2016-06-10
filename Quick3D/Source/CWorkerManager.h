
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
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //! Ajoute un travailleur au pool
    void addWorker(CWorker* pWorker);

    //! Retire un travailleur du pool
    void removeWorker(CWorker* pWorker);

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

protected:

    //! Conctructeur par défaut
    CWorkerManager();

    //! Destructeur
    virtual ~CWorkerManager();

    //! Implémentation de la méthode run() de QThread
    virtual void run();

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QMutex              m_mMutex;			// Mutex de protection des accès concurrents
    QVector<CWorker*>   m_vWorkers;			// Pool de travailleurs
    bool                m_bStopRequested;	// Indique si un arrêt du travail est demandé
};
