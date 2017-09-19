
// Application
#include "CTrajectorable.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CTrajectorable::CTrajectorable(C3DScene* pScene)
    : CMesh(pScene)
    , m_bTrajectoryEnabled(true)
    , m_dSpeedMS(2.0)
    , m_dTurnSpeedDS(5.0)
    , m_aRotation(10)
{
}

//-------------------------------------------------------------------------------------------------

CTrajectorable::~CTrajectorable()
{
}

//-------------------------------------------------------------------------------------------------

void CTrajectorable::update(double dDeltaTime)
{
    CMesh::update(dDeltaTime);

    if (isRootObject() && m_bTrajectoryEnabled)
    {
        m_tTrajectory.processObject(this, dDeltaTime);
    }
}

//-------------------------------------------------------------------------------------------------

void CTrajectorable::resetTrajectory()
{
    m_tTrajectory.reset();

    setGeoloc(m_tTrajectory.position());
    setRotation(m_tTrajectory.rotation());
}
