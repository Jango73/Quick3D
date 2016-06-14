
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QDateTime>

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CExpendable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur d'apr�s une sc�ne
    CExpendable(int iSecondsToExpendability = 60);

    //! Destructeur
    virtual ~CExpendable();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setSecondsToExpendability(int iSeconds);

    //!
    void setUsedNow();

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual bool isExpendable() const;

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    QDateTime   m_tLastUsed;
    int         m_iSecondsToExpendability;
};