
// qt-plus
#include "CLogger.h"

// Std
#include <algorithm>

// Qt
#include <QMutexLocker>

// Application
#include "C3DScene.h"
#include "CGeoFrame.h"
#include "CComponentFactory.h"
#include "CWorkerManager.h"
#include "CWorldTerrain.h"
#include "CWorldLodPolicy.h"
#include "CRenderLayer.h"
#include "CTreeGenerator.h"
#include "CTiledMaterial.h"
#include "CGeneratedField.h"
#include "CSRTMField.h"
#include "CHGTField.h"
#include "CBILField.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the chunk identity as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode SCollectedChunkInfo::snapshotNode() const
{
    CXMLNode xNode("chunks");

    xNode.attributes()["address"] = address;
    xNode.attributes()["latitude"] = QString::number(latitude, 'g', 10);
    xNode.attributes()["longitude"] = QString::number(longitude, 'g', 10);
    xNode.attributes()["sizelatitude"] = QString::number(sizeLatitude, 'g', 10);
    xNode.attributes()["sizelongitude"] = QString::number(sizeLongitude, 'g', 10);

    return xNode;
}

/***************************************************************************/
/*!
    Returns the level readiness as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode SChunkPathInfo::snapshotNode() const
{
    CXMLNode xNode("path");

    xNode.attributes()["level"] = QString::number(level);
    xNode.attributes()["latitude"] = QString::number(latitude, 'g', 10);
    xNode.attributes()["longitude"] = QString::number(longitude, 'g', 10);
    xNode.attributes()["sizelatitude"] = QString::number(sizeLatitude, 'g', 10);
    xNode.attributes()["sizelongitude"] = QString::number(sizeLongitude, 'g', 10);
    xNode.attributes()["selfready"] = selfReady == true ? "true" : "false";
    xNode.attributes()["children"] = QString::number(childCount);
    xNode.attributes()["drawable"] = QString::number(drawableChildren);
    xNode.attributes()["stay"] = stay == true ? "true" : "false";

    return xNode;
}

//-------------------------------------------------------------------------------

/*!
    Records the quadtree readiness on the path from the root to \a gCamera, retained for snapshots. \br\br
    Descent follows the child containing the camera like the paint recursion does:
    the first level with no ready chunk and no drawable child on this path is
    where the frame gives up and falls back.
*/
void CWorldTerrain::recordCameraPath(const CGeoloc& gCamera)
{
    QVector<SChunkPathInfo> vPath;

    QSP<CWorldChunk> pChunk = m_pRoot;
    int iLevel = m_iLevels;

    while (pChunk != nullptr && iLevel >= 0)
    {
        SChunkPathInfo tInfo;
        tInfo.level = iLevel;
        tInfo.latitude = pChunk->geoloc().Latitude;
        tInfo.longitude = pChunk->geoloc().Longitude;
        tInfo.sizeLatitude = pChunk->size().Latitude;
        tInfo.sizeLongitude = pChunk->size().Longitude;
        SChildStatistics tStats = pChunk->childStatistics();

        tInfo.selfReady = pChunk->terrain() != nullptr && pChunk->terrain()->isOK();
        tInfo.childCount = tStats.childCount;
        tInfo.drawableChildren = tStats.drawableChildren;
        tInfo.stay = enoughDetail(pChunk, gCamera, iLevel);

        QSP<CWorldChunk> pNext;

        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            if (pChild == nullptr) continue;

            if (pNext == nullptr && pChild->worldBounds().containsSpherical(gCamera))
            {
                pNext = pChild;
            }
        }

        vPath.append(tInfo);

        pChunk = pNext;
        iLevel--;
    }

    {
        QMutexLocker tLocker(&m_mSnapshotMutex);
        m_vLastCameraPath = vPath;
    }
}

//-------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the per-layer chunk identities retained from the last paint.
*/
/***************************************************************************/
QMap<int, QVector<SCollectedChunkInfo> > CWorldTerrain::lastCollectedChunks() const
{
    QMutexLocker tLocker(&m_mSnapshotMutex);

    return m_mLastCollectedChunks;
}

/***************************************************************************/
/*!
    Returns the quadtree readiness retained from the last paint.
*/
/***************************************************************************/
QVector<SChunkPathInfo> CWorldTerrain::lastCameraPath() const
{
    QMutexLocker tLocker(&m_mSnapshotMutex);

    return m_vLastCameraPath;
}

/***************************************************************************/
/*!
    Returns the camera position retained from the last paint.
*/
/***************************************************************************/
CGeoloc CWorldTerrain::lastCameraGeoloc() const
{
    QMutexLocker tLocker(&m_mSnapshotMutex);

    return m_gLastCameraGeoloc;
}

/***************************************************************************/
/*!
    Returns the last collect counts and the quadtree readiness on the camera path as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode CWorldTerrain::snapshotNode()
{
    QMap<int, QVector<SCollectedChunkInfo> > mCollected;
    QVector<SChunkPathInfo> vPath;

    {
        QMutexLocker tLocker(&m_mSnapshotMutex);
        mCollected = m_mLastCollectedChunks;
        vPath = m_vLastCameraPath;
    }

    CXMLNode xNode("terrain");

    fillSnapshotBaseNode(xNode);

    foreach (int iLayer, mCollected.keys())
    {
        CXMLNode xLayer("collect");

        xLayer.attributes()["layer"] = QString::number(iLayer);

        foreach (const SCollectedChunkInfo& tChunk, mCollected[iLayer])
        {
            xLayer << tChunk.snapshotNode();
        }

        xNode << xLayer;
    }

    foreach (const SChunkPathInfo& tLevel, vPath)
    {
        xNode << tLevel.snapshotNode();
    }

    return xNode;
}
