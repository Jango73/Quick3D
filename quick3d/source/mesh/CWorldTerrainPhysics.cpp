
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
    Registers ready chunk grids as physics height fields, drops stale ones
*/
void CWorldTerrain::syncPhysicsHeightfields()
{
    if (m_pRoot == nullptr || m_pScene == nullptr)
    {
        return;
    }

    if (m_pScene->physicsWorld() == nullptr)
    {
        return;
    }

    syncPhysicsHeightfieldsRecurse(m_pRoot);
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers \a pChunk 's grid once its terrain is ready, drops the height
    field when the grid is gone, then visits child chunks
*/
void CWorldTerrain::syncPhysicsHeightfieldsRecurse(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr || m_pScene == nullptr)
    {
        return;
    }

    CPhysicsWorld* pWorld = m_pScene->physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    m_pScene->ensurePhysicsReference(pChunk->geoloc());

    if (chunkHeightfieldIndex(pChunk) < 0)
    {
        // Not registered yet: copy the grid once and register it
        QVector<float> vGrid;
        int iPoints = 0;

        if (pChunk->terrainGrid(vGrid, iPoints) == true)
        {
            SGeoFrame cFrame = CGeoFrame::heightfieldFrame(
                pChunk->geoloc(),
                pChunk->size(),
                iPoints,
                m_pScene->physicsReference().toVector3()
                );

            int iHeightfield = pWorld->addHeightfield(vGrid, iPoints, cFrame.SpacingXM, cFrame.SpacingZM, cFrame.Position, cFrame.Rotation);

            if (iHeightfield < 0)
            {
                LOG_METHOD_WARNING("Chunk height field registration failed");
            }

            setChunkHeightfieldIndex(pChunk, iHeightfield);
        }
        else
        {
            removeChunkHeightfield(pChunk);
        }
    }
    else if (pChunk->terrainGridReady() == false)
    {
        // Already registered: cheap liveness check only, no grid copy
        removeChunkHeightfield(pChunk);
    }

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            syncPhysicsHeightfieldsRecurse(pChild);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes the physics height field of \a pChunk when registered
*/
void CWorldTerrain::removeChunkHeightfield(QSP<CWorldChunk> pChunk)
{
    int iHeightfield = chunkHeightfieldIndex(pChunk);

    if (pChunk == nullptr || iHeightfield < 0)
    {
        return;
    }

    if (m_pScene != nullptr && m_pScene->physicsWorld() != nullptr)
    {
        m_pScene->physicsWorld()->removeHeightfield(iHeightfield);
    }

    setChunkHeightfieldIndex(pChunk, -1);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the registered height field id of \a pChunk, -1 when none
*/
int CWorldTerrain::chunkHeightfieldIndex(QSP<CWorldChunk> pChunk) const
{
    if (pChunk == nullptr)
    {
        return -1;
    }

    return m_mPhysicsHeightfields.value(pChunk.data(), -1);
}

//-------------------------------------------------------------------------------------------------

/*!
    Records \a iIndex as the height field of \a pChunk, -1 unregisters without removal
*/
void CWorldTerrain::setChunkHeightfieldIndex(QSP<CWorldChunk> pChunk, int iIndex)
{
    if (pChunk == nullptr)
    {
        return;
    }

    if (iIndex < 0)
    {
        m_mPhysicsHeightfields.remove(pChunk.data());
    }
    else
    {
        m_mPhysicsHeightfields.insert(pChunk.data(), iIndex);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes every registered chunk height field from the physics world
*/
void CWorldTerrain::clearPhysicsHeightfields()
{
    if (m_mPhysicsHeightfields.count() == 0)
    {
        return;
    }

    CPhysicsWorld* pWorld = nullptr;

    if (m_pScene != nullptr)
    {
        pWorld = m_pScene->physicsWorld();
    }

    // Note: keys are copied, removal during iteration is safe
    foreach (const CWorldChunk* pChunk, m_mPhysicsHeightfields.keys())
    {
        int iHeightfield = m_mPhysicsHeightfields.value(pChunk, -1);

        if (pWorld != nullptr && iHeightfield >= 0)
        {
            pWorld->removeHeightfield(iHeightfield);
        }
    }

    m_mPhysicsHeightfields.clear();
}
