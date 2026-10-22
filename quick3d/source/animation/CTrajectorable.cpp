
// Application
#include "CTrajectorable.h"
#include "CMesh.h"
#include "C3DScene.h"

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

//-------------------------------------------------------------------------------------------------

/*!
    Returns a JSON document with the movement state (geoloc from the base, rotation, speeds).
*/
QString CTrajectorable::snapshotJson()
{
    return QString("{%1,%2}").arg(snapshotBaseJson()).arg(snapshotMovementJson());
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the shared rotation and speed fields used by snapshotJson() overrides.
*/
QString CTrajectorable::snapshotMovementJson() const
{
    Math::CVector3 vRotation = rotation();

    bool bControlled = m_pScene != nullptr
        && m_pScene->controller() != nullptr
        && m_pController != nullptr
        && m_pScene->controller() == m_pController;

    return QString("\"rotationx\":%1,\"rotationy\":%2,\"rotationz\":%3,\"speedms\":%4,\"turnspeedds\":%5,\"controlled\":%6")
        .arg(vRotation.X, 0, 'g', 10)
        .arg(vRotation.Y, 0, 'g', 10)
        .arg(vRotation.Z, 0, 'g', 10)
        .arg(m_dSpeedMS, 0, 'g', 10)
        .arg(m_dTurnSpeedDS, 0, 'g', 10)
        .arg(bControlled == true ? "true" : "false");
}
