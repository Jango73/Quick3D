
#pragma once

// Qt
#include <QString>

//-------------------------------------------------------------------------------------------------

class C3DSceneStatistics
{
public:

    C3DSceneStatistics();

    void reset();

    //! Returns the counters as a JSON object
    QString snapshotJson() const;

    int     m_iNumMeshesDrawn;
    int     m_iNumPolysDrawn;
    int     m_iNumChunksDrawn;
    int     m_iNumFrustumTests;
    int     m_iNumFrustumPassed;
    int     m_iNumFrustumOutOfRange;
    int     m_iNumRayIntersectionTests;
};
