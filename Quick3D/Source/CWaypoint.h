
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CQ3DConstants.h"
#include "CGeoloc.h"

//-------------------------------------------------------------------------------------------------

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
    wtTopOfDescent
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
    CWaypoint(EWaypointType eType, QString sName, CGeoloc gGeoloc, double dFrequency, bool bGenerated = false);

    //!
    virtual ~CWaypoint();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CWaypoint; }

    //!
    QString getName() const { return m_sName; }

    //!
    EWaypointType getType() const { return m_eType; }

    //!
    CGeoloc getGeoloc() const { return m_gGeoloc; }

    //!
    CGeoloc getEndGeoloc() const { return m_gEndGeoloc; }

    //!
    double getFrequency() const { return m_dFrequency; }

    //!
    bool isGenerated() const { return m_bGenerated; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    EWaypointType   m_eType;
    QString         m_sName;
    CGeoloc         m_gGeoloc;
    CGeoloc         m_gEndGeoloc;
    double          m_dFrequency;
    bool            m_bGenerated;
};
