
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
    Returns the movement state for the current frame as a node, JSON through CXMLNode.
*/
CXMLNode CTrajectorable::snapshotNode()
{
    CXMLNode xNode("component");

    fillSnapshotBaseNode(xNode);
    fillSnapshotMovementNode(xNode);

    return xNode;
}

//-------------------------------------------------------------------------------------------------

/*!
    Fills the shared rotation and speed attributes used by snapshotNode() overrides.
*/
void CTrajectorable::fillSnapshotMovementNode(CXMLNode& xNode) const
{
    Math::CVector3 vRotation = rotation();

    bool bControlled = m_pScene != nullptr
        && m_pScene->controller() != nullptr
        && m_pController != nullptr
        && m_pScene->controller() == m_pController;

    xNode.attributes()["rotationx"] = QString::number(vRotation.X, 'g', 10);
    xNode.attributes()["rotationy"] = QString::number(vRotation.Y, 'g', 10);
    xNode.attributes()["rotationz"] = QString::number(vRotation.Z, 'g', 10);
    xNode.attributes()["speedms"] = QString::number(m_dSpeedMS, 'g', 10);
    xNode.attributes()["turnspeedds"] = QString::number(m_dTurnSpeedDS, 'g', 10);
    xNode.attributes()["controlled"] = bControlled == true ? "true" : "false";
}
