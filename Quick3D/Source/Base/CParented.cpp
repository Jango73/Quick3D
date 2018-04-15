
// qt-plus
#include "CLogger.h"

// Application
#include "CParented.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CParented
    \brief A base class for an object that has a parent.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CWaypoint with its default parameters.
*/
CParented::CParented()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CWaypoint.
*/
CParented::~CParented()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the parent name to \a sName.
*/
void CParented::setParentName(const QString& sName)
{
	m_sParentName = sName;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the parent name.
*/
const QString& CParented::parentName() const
{
	return m_sParentName;
}
