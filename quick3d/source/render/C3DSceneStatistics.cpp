
// Application
#include "C3DSceneStatistics.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Builds zeroed counters.
*/
/***************************************************************************/
C3DSceneStatistics::C3DSceneStatistics()
    : m_iNumMeshesDrawn(0)
    , m_iNumPolysDrawn(0)
    , m_iNumChunksDrawn(0)
    , m_iNumFrustumTests(0)
    , m_iNumFrustumPassed(0)
    , m_iNumFrustumOutOfRange(0)
    , m_iNumRayIntersectionTests(0)
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Resets every counter to zero.
*/
/***************************************************************************/
void C3DSceneStatistics::reset()
{
    m_iNumMeshesDrawn = 0;
    m_iNumPolysDrawn = 0;
    m_iNumChunksDrawn = 0;
    m_iNumFrustumTests = 0;
    m_iNumFrustumPassed = 0;
    m_iNumFrustumOutOfRange = 0;
    m_iNumRayIntersectionTests = 0;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the counters as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode C3DSceneStatistics::snapshotNode() const
{
    CXMLNode xNode("statistics");

    xNode.attributes()["meshes"] = QString::number(m_iNumMeshesDrawn);
    xNode.attributes()["polys"] = QString::number(m_iNumPolysDrawn);
    xNode.attributes()["chunks"] = QString::number(m_iNumChunksDrawn);
    xNode.attributes()["tests"] = QString::number(m_iNumFrustumTests);
    xNode.attributes()["passed"] = QString::number(m_iNumFrustumPassed);
    xNode.attributes()["outofrange"] = QString::number(m_iNumFrustumOutOfRange);
    xNode.attributes()["raytests"] = QString::number(m_iNumRayIntersectionTests);

    return xNode;
}
