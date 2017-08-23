
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

enum EWaypointType
{
    wtNone,
    wtFix,
    wtVOR,
    wtVORTAC,
    wtNDB,
    wtAirport,
    wtRunway,
    wtBottomOfClimb,
    wtTopOfClimb,
    wtTopOfDescent,
    wtBottomOfDescent
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWaypoint
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CWaypoint();

    //!
    CWaypoint(EWaypointType eType, QString sName, CGeoloc gGeoloc, double dFrequency_MHz, bool bGenerated = false);

    //!
    virtual ~CWaypoint();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setMinimumAltitude_m(double dValue);

    //!
    void setMaximumAltitude_m(double dValue);

    //!
    void setSelectedAltitude_m(double dValue);

    //!
    void setComputedAltitude_m(double dValue);

    //!
    void setComputedSpeed_ms(double dValue);

    //!
    void setGenerated(bool bValue);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CWaypoint; }

    //!
    QString name() const { return m_sName; }

    //!
    EWaypointType type() const { return m_eType; }

    //!
    CGeoloc geoloc() const { return m_gGeoloc; }

    //!
    CGeoloc endGeoloc() const { return m_gEndGeoloc; }

    //!
    double frequency_MHz() const { return m_dFrequency_MHz; }

    //!
    double minimumAltitude_m() const { return m_dMinimumAltitude_m; }

    //!
    double maximumAltitude_m() const { return m_dMaximumAltitude_m; }

    //!
    double selectedAltitude_m() const { return m_dSelectedAltitude_m; }

    //!
    bool isGenerated() const { return m_bGenerated; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    // General data
    EWaypointType   m_eType;
    QString         m_sName;
    CGeoloc         m_gGeoloc;
    CGeoloc         m_gEndGeoloc;
    double          m_dFrequency_MHz;

    // Constraints
    double          m_dMinimumAltitude_m;
    double          m_dMaximumAltitude_m;

    // Selected and computed data
    double          m_dSelectedAltitude_m;
    double          m_dComputedAltitude_m;
    double          m_dComputedSpeed_ms;
    bool            m_bGenerated;
};
