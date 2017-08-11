
// Fondations
#include "CLogger.h"

// Application
#include "CNamed.h"

//-------------------------------------------------------------------------------------------------

CNamed::CNamed()
{
}

//-------------------------------------------------------------------------------------------------

CNamed::CNamed(const QString & sName)
    : m_sName(sName)
{
}

//-------------------------------------------------------------------------------------------------

CNamed::~CNamed()
{
}

//-------------------------------------------------------------------------------------------------

void CNamed::setName(const QString& sName)
{
    m_sName = sName;
}

//-------------------------------------------------------------------------------------------------

const QString& CNamed::name() const
{
    return m_sName;
}
