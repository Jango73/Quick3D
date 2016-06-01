
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
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CWaypoint();

	//!
	CWaypoint(EWaypointType eType, QString sName, Math::CGeoloc gGeoloc, double dFrequency, bool bGenerated = false);

	//!
	virtual ~CWaypoint();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CWaypoint; }

	//!
	QString getName() const { return m_sName; }

	//!
	EWaypointType getType() const { return m_eType; }

	//!
	Math::CGeoloc getGeoloc() const { return m_gGeoloc; }

	//!
	Math::CGeoloc getEndGeoloc() const { return m_gEndGeoloc; }

	//!
	double getFrequency() const { return m_dFrequency; }

	//!
	bool isGenerated() const { return m_bGenerated; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	EWaypointType	m_eType;
	QString			m_sName;
	Math::CGeoloc	m_gGeoloc;
	Math::CGeoloc	m_gEndGeoloc;
	double			m_dFrequency;
	bool			m_bGenerated;
};
