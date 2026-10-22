
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
};
