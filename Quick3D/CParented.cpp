
// Application
#include "CLogManager.h"
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

const QString& CParented::getParentName() const
{
	return m_sParentName;
}
