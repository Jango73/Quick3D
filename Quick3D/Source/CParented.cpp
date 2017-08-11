
// Application
#include "CLogger.h"
#include "CParented.h"

//-------------------------------------------------------------------------------------------------

CParented::CParented()
{
}

//-------------------------------------------------------------------------------------------------

CParented::~CParented()
{
}

//-------------------------------------------------------------------------------------------------

void CParented::setParentName(const QString& sName)
{
	m_sParentName = sName;
}

//-------------------------------------------------------------------------------------------------

const QString& CParented::parentName() const
{
	return m_sParentName;
}
