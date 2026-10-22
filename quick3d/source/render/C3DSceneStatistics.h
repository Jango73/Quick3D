
#pragma once

// Qt
#include <QString>

class C3DSceneStatistics
{
public:

    C3DSceneStatistics()
        : m_iNumMeshesDrawn(0)
        , m_iNumPolysDrawn(0)
        , m_iNumChunksDrawn(0)
        , m_iNumFrustumTests(0)
        , m_iNumFrustumPassed(0)
        , m_iNumFrustumOutOfRange(0)
        , m_iNumRayIntersectionTests(0)
    {
    }

    void reset()
    {
        m_iNumMeshesDrawn = 0;
        m_iNumPolysDrawn = 0;
        m_iNumChunksDrawn = 0;
        m_iNumFrustumTests = 0;
        m_iNumFrustumPassed = 0;
        m_iNumFrustumOutOfRange = 0;
        m_iNumRayIntersectionTests = 0;
    }

    //! Returns the counters as a JSON object
    QString snapshotJson() const
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

    int     m_iNumMeshesDrawn;
    int     m_iNumPolysDrawn;
    int     m_iNumChunksDrawn;
    int     m_iNumFrustumTests;
    int     m_iNumFrustumPassed;
    int     m_iNumFrustumOutOfRange;
    int     m_iNumRayIntersectionTests;
};
