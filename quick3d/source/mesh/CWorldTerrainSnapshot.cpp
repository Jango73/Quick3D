
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
#include "CJsonBuilder.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Returns the chunk identity as a JSON object.
*/
/***************************************************************************/
QString SCollectedChunkInfo::snapshotJson() const
{
    return QString("{\"address\":\"%1\",\"latitude\":%2,\"longitude\":%3,\"sizelatitude\":%4,\"sizelongitude\":%5}")
        .arg(address)
        .arg(latitude, 0, 'g', 10)
        .arg(longitude, 0, 'g', 10)
        .arg(sizeLatitude, 0, 'g', 10)
        .arg(sizeLongitude, 0, 'g', 10);
}

/***************************************************************************/
/*!
    Returns the level readiness as a JSON object.
*/
/***************************************************************************/
QString SChunkPathInfo::snapshotJson() const
{
    return QString("{\"level\":%1,\"latitude\":%2,\"longitude\":%3,\"sizelatitude\":%4,\"sizelongitude\":%5,"
        "\"selfready\":%6,\"children\":%7,\"drawable\":%8,\"stay\":%9}")
        .arg(level)
        .arg(latitude, 0, 'g', 10)
        .arg(longitude, 0, 'g', 10)
        .arg(sizeLatitude, 0, 'g', 10)
        .arg(sizeLongitude, 0, 'g', 10)
        .arg(selfReady == true ? "true" : "false")
        .arg(childCount)
        .arg(drawableChildren)
        .arg(stay == true ? "true" : "false");
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
    Returns a JSON document with the last collect counts and the quadtree readiness on the camera path.
*/
/***************************************************************************/
QString CWorldTerrain::snapshotJson()
{
    QMap<int, QVector<SCollectedChunkInfo> > mCollected;
    QVector<SChunkPathInfo> vPath;

    {
        QMutexLocker tLocker(&m_mSnapshotMutex);
        mCollected = m_mLastCollectedChunks;
        vPath = m_vLastCameraPath;
    }

    QStringList vLayers;

    foreach (int iLayer, mCollected.keys())
    {
        QStringList vChunks;

        foreach (const SCollectedChunkInfo& tChunk, mCollected[iLayer])
        {
            vChunks.append(tChunk.snapshotJson());
        }

        vLayers.append(QString("{\"layer\":%1,\"chunks\":%2}").arg(iLayer).arg(CJsonBuilder::joinArray(vChunks)));
    }

    QStringList vLevels;

    foreach (const SChunkPathInfo& tLevel, vPath)
    {
        vLevels.append(tLevel.snapshotJson());
    }

    return QString("{%1,\"collect\":%2,\"path\":%3}")
        .arg(snapshotBaseJson())
        .arg(CJsonBuilder::joinArray(vLayers))
        .arg(CJsonBuilder::joinArray(vLevels));
}
