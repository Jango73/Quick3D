
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
    Returns the counters as a JSON object.
*/
/***************************************************************************/
QString C3DSceneStatistics::snapshotJson() const
{
    return QString("{\"meshes\":%1,\"polys\":%2,\"chunks\":%3,\"tests\":%4,\"passed\":%5,\"outofrange\":%6,\"raytests\":%7}")
        .arg(m_iNumMeshesDrawn)
        .arg(m_iNumPolysDrawn)
        .arg(m_iNumChunksDrawn)
        .arg(m_iNumFrustumTests)
        .arg(m_iNumFrustumPassed)
        .arg(m_iNumFrustumOutOfRange)
        .arg(m_iNumRayIntersectionTests);
}
