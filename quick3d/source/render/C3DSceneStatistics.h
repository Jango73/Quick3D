
#pragma once

// Qt
#include <QString>

// qt-plus
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

class C3DSceneStatistics
{
public:

    C3DSceneStatistics();

    void reset();

    //! Returns the counters as a node, JSON through CXMLNode
    CXMLNode snapshotNode() const;

    int     m_iNumMeshesDrawn;
    int     m_iNumPolysDrawn;
    int     m_iNumChunksDrawn;
    int     m_iNumFrustumTests;
    int     m_iNumFrustumPassed;
    int     m_iNumFrustumOutOfRange;
    int     m_iNumRayIntersectionTests;

    // Accumulated nanoseconds per layer pass, flushed to CTimeSampler once per pass.
    // Render thread only, no locking. Not part of the snapshot node.
    qint64  m_iChunkTerrainNs;
    qint64  m_iChunkWaterNs;
    qint64  m_iChunkBoundedNs;
    qint64  m_iMaterialActivateNs;
    qint64  m_iMeshBindStateNs;
    qint64  m_iMeshDrawNs;
};
