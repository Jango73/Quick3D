
#pragma once

// Qt
#include <QString>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

class CTrajectorable;

class QUICK3D_EXPORT CTrajectory
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CTrajectory(bool bAutoOrientation = true);

    //!
    virtual ~CTrajectory();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setPosition(CGeoloc Position);

    //!
    void setRotation(Math::CVector3 Rotation);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CGeoloc position();

    //!
    Math::CVector3 rotation();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void reset();

    //!
    void addPoint(CGeoloc vPoint);

    //!
    void processObject(CTrajectorable* pVehicle, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CGeoloc             m_gPosition;
    Math::CVector3      m_vRotation;
    QVector<CGeoloc>    m_vPoints;
    int                 m_iCurrentPoint;
    bool                m_bAutoOrientation;
};
