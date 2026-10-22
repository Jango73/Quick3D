
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

/*!
    Destroys the terrain, water and height field of \a pChunk and every node below it
*/
void CWorldTerrain::destroyChunkSubtree(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr) return;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        destroyChunkSubtree(QSP_CAST(CWorldChunk, pChildComponent));
    }

    removeChunkHeightfield(pChunk);

    pChunk->clearLinks(m_pScene);
    pChunk->clearTerrain();
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint in order to collect any useless chunks.
*/
void CWorldTerrain::collectGarbage()
{
    if (m_pRoot != nullptr)
    {
        collectGarbageRecurse(m_pRoot, m_iLevels, lastCameraGeoloc());
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Applies the LOD policy bottom-up: a node covered by four ready children
    loses its patch, unused children of a covering node go away. \br\br
    \a iLevel is the depth of \a pChunk in the chunk tree.
*/
void CWorldTerrain::collectGarbageRecurse(QSP<CWorldChunk> pChunk, int iLevel)
{
    collectGarbageRecurse(pChunk, iLevel, lastCameraGeoloc());
}

/***************************************************************************/
/*!
    Applies the LOD policy bottom-up: a node covered by four ready children
    loses its patch, unused children of a covering node go away. \br\br
    \a pChunk is the chunk under visit, \a iLevel its depth in the chunk tree,
    \a gCamera the camera position retained from the last paint, copied once by
    collectGarbage() so the whole pass reads one stable value.
*/
/***************************************************************************/
void CWorldTerrain::collectGarbageRecurse(QSP<CWorldChunk> pChunk, int iLevel, const CGeoloc& gCamera)
{
    if (pChunk == nullptr) return;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            collectGarbageRecurse(pChild, iLevel - 1, gCamera);
        }
    }

    SChildStatistics tStats = pChunk->childStatistics();
    bool bChildrenGoingAway = tStats.childCount > 0;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild == nullptr) continue;

        // Going away means nobody needs the zone: out of range, or dead weight.
        // Workers in flight keep their subtree, and needed children stay.
        bool bChildNeeded = CWorldLodPolicy::isNeeded(pChild->originalGeoloc(), pChild->originalSize(), gCamera);

        if ((bChildNeeded == false || pChild->isEmpty()) == false || pChild->subtreeHasWorker())
        {
            bChildrenGoingAway = false;
        }
    }

    bool bAllChildrenReady = CWorldLodPolicy::allChildrenReady(tStats.drawableChildren, tStats.childCount);
    bool bCovered = bAllChildrenReady || tStats.childCount == 0;
    bool bNeeded = CWorldLodPolicy::isNeeded(pChunk->originalGeoloc(), pChunk->originalSize(), gCamera);

    // R2: covered and out of range, the own patch may go unless protected.
    // Only the patch is freed, the tree links stay. No timer: needed is the
    // policy visibility, useless patches go on the next garbage pass.
    if (pChunk->terrain() != nullptr
        && CWorldLodPolicy::mayDestroySelf(bCovered, iLevel, m_iLevels, bNeeded))
    {
        removeChunkHeightfield(pChunk);

        pChunk->clearTerrain();
    }

    bool bSelfReady = pChunk->terrain() != nullptr && pChunk->terrain()->isOK();

    // R3: unneeded children go away, the parent covers them
    if (bAllChildrenReady == false && bChildrenGoingAway)
    {
        if (bSelfReady == false && bNeeded)
        {
            // Parent cover missing but needed: rebuild it, prune the children on a later pass
            ensureChunkCover(pChunk, iLevel);
        }
        else
        {
            // Parent covers, or nobody needs the zone: prune
            for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);

                destroyChunkSubtree(pChild);

                pChunk->childComponents().remove(iIndex);
                iIndex--;
            }
        }
    }
}
