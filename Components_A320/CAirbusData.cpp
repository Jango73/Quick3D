
// Quick3D
#include "../Quick3D/CLogManager.h"

// Application
#include "CAirbusData.h"

//-------------------------------------------------------------------------------------------------

CAirbusData::CAirbusData()
: m_eID(adNone)
{
}

//-------------------------------------------------------------------------------------------------

CAirbusData::CAirbusData(const QString& sSource, EAirbusData eID, QVariant vData)
: m_sSource(sSource)
, m_eID(eID)
, m_vData(vData)
{
}

//-------------------------------------------------------------------------------------------------

CAirbusData::~CAirbusData()
{
}
