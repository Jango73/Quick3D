
#pragma once

class C3DSceneStatistics
{
public:

    C3DSceneStatistics()
        : m_iNumMeshesDrawn(0)
        , m_iNumPolysDrawn(0)
        , m_iNumChunksDrawn(0)
        , m_iNumFrustumTests(0)
        , m_iNumRayIntersectionTests(0)
    {
    }

    void reset()
    {
        m_iNumMeshesDrawn = 0;
        m_iNumPolysDrawn = 0;
        m_iNumChunksDrawn = 0;
        m_iNumFrustumTests = 0;
        m_iNumRayIntersectionTests = 0;
    }

    int     m_iNumMeshesDrawn;
    int     m_iNumPolysDrawn;
    int     m_iNumChunksDrawn;
    int     m_iNumFrustumTests;
    int     m_iNumRayIntersectionTests;
};
