
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
CXMLNode COLLECTED_CHUNK_INFO::snapshotNode() const
{
    CXMLNode xNode("chunks");

    xNode.attributes()["address"] = m_sAddress;
    xNode.attributes()["latitude"] = QString::number(m_dLatitude, 'g', 10);
    xNode.attributes()["longitude"] = QString::number(m_dLongitude, 'g', 10);
    xNode.attributes()["sizelatitude"] = QString::number(m_dSizeLatitude, 'g', 10);
    xNode.attributes()["sizelongitude"] = QString::number(m_dSizeLongitude, 'g', 10);

    return xNode;
}

/***************************************************************************/
/*!
    Returns the level readiness as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode CHUNK_PATH_INFO::snapshotNode() const
{
    CXMLNode xNode("path");

    xNode.attributes()["level"] = QString::number(m_iLevel);
    xNode.attributes()["latitude"] = QString::number(m_dLatitude, 'g', 10);
    xNode.attributes()["longitude"] = QString::number(m_dLongitude, 'g', 10);
    xNode.attributes()["sizelatitude"] = QString::number(m_dSizeLatitude, 'g', 10);
    xNode.attributes()["sizelongitude"] = QString::number(m_dSizeLongitude, 'g', 10);
    xNode.attributes()["selfready"] = m_bSelfReady == true ? "true" : "false";
    xNode.attributes()["children"] = QString::number(m_iChildCount);
    xNode.attributes()["drawable"] = QString::number(m_iDrawableChildren);
    xNode.attributes()["stay"] = m_bStay == true ? "true" : "false";

    return xNode;
}

//-------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the height field identity as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode REGISTERED_HEIGHTFIELD_INFO::snapshotNode() const
{
    CXMLNode xNode("heightfields");

    xNode.attributes()["id"] = QString::number(m_iHeightfield);
    xNode.attributes()["latitude"] = QString::number(m_dLatitude, 'g', 10);
    xNode.attributes()["longitude"] = QString::number(m_dLongitude, 'g', 10);
    xNode.attributes()["sizelatitude"] = QString::number(m_dSizeLatitude, 'g', 10);
    xNode.attributes()["sizelongitude"] = QString::number(m_dSizeLongitude, 'g', 10);

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
    QVector<CHUNK_PATH_INFO> vPath;

    QSP<CWorldChunk> pChunk = m_pRoot;
    int iLevel = m_iLevels;

    while (pChunk != nullptr && iLevel >= 0)
    {
        CHUNK_PATH_INFO tInfo;
        tInfo.m_iLevel = iLevel;
        tInfo.m_dLatitude = pChunk->geoloc().Latitude;
        tInfo.m_dLongitude = pChunk->geoloc().Longitude;
        tInfo.m_dSizeLatitude = pChunk->size().Latitude;
        tInfo.m_dSizeLongitude = pChunk->size().Longitude;
        SChildStatistics tStats = pChunk->childStatistics();

        tInfo.m_bSelfReady = pChunk->terrain() != nullptr && pChunk->terrain()->isOK();
        tInfo.m_iChildCount = tStats.childCount;
        tInfo.m_iDrawableChildren = tStats.drawableChildren;
        tInfo.m_bStay = enoughDetail(pChunk, gCamera, iLevel);

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
QMap<QString, QVector<COLLECTED_CHUNK_INFO> > CWorldTerrain::lastCollectedChunks() const
{
    QMutexLocker tLocker(&m_mSnapshotMutex);

    return m_mLastCollectedChunks;
}

/***************************************************************************/
/*!
    Returns the quadtree readiness retained from the last paint.
*/
/***************************************************************************/
QVector<CHUNK_PATH_INFO> CWorldTerrain::lastCameraPath() const
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
    Returns the physics height field identities retained from the last sync.
*/
/***************************************************************************/
QVector<REGISTERED_HEIGHTFIELD_INFO> CWorldTerrain::lastHeightfields() const
{
    QMutexLocker tLocker(&m_mSnapshotMutex);

    return m_vLastHeightfields;
}

/***************************************************************************/
/*!
    Returns the last collect counts and the quadtree readiness on the camera path as a node, JSON through CXMLNode.
*/
/***************************************************************************/
CXMLNode CWorldTerrain::snapshotNode()
{
    QMap<QString, QVector<COLLECTED_CHUNK_INFO> > mCollected;
    QVector<CHUNK_PATH_INFO> vPath;
    QVector<REGISTERED_HEIGHTFIELD_INFO> vHeightfields;

    {
        QMutexLocker tLocker(&m_mSnapshotMutex);
        mCollected = m_mLastCollectedChunks;
        vPath = m_vLastCameraPath;
        vHeightfields = m_vLastHeightfields;
    }

    CXMLNode xNode("terrain");

    fillSnapshotBaseNode(xNode);

    foreach (const QString& sLayer, mCollected.keys())
    {
        CXMLNode xLayer("collect");

        xLayer.attributes()["layer"] = sLayer;

        foreach (const COLLECTED_CHUNK_INFO& tChunk, mCollected[sLayer])
        {
            xLayer << tChunk.snapshotNode();
        }

        xNode << xLayer;
    }

    foreach (const CHUNK_PATH_INFO& tLevel, vPath)
    {
        xNode << tLevel.snapshotNode();
    }

    foreach (const REGISTERED_HEIGHTFIELD_INFO& tHeightfield, vHeightfields)
    {
        xNode << tHeightfield.snapshotNode();
    }

    return xNode;
}
