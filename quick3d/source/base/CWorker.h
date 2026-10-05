
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

    //! Retourne vrai si le travail a commenc�
    bool isStarted() { return m_bStarted; }

    //! Retourne vrai si le travail est termin�
    bool isFinished() { return m_bFinished; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! M�thode de travail
    virtual void work();

    //! Annule le travail de l'objet
    void abort();

    //! Remet le travailleur en �tat d'�tre remis dans la file apr�s une ex�cution
    void resetForReuse();

    //! Ex�cute un sleep
    void doSleep(int ms);

    //-------------------------------------------------------------------------------------------------
    // Inherited methods prot�g�es
    //-------------------------------------------------------------------------------------------------

protected:

    //! Impl�mentation de la m�thode run() de QThread
    virtual void run();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    std::atomic<bool>   m_bStopRequested;   // Indique si l'arr�t du travail est demand�

private:

    std::atomic<bool>   m_bStarted;         // Indique si le travail a commenc�
    std::atomic<bool>   m_bFinished;        // Indique si le travail est termin�
};
