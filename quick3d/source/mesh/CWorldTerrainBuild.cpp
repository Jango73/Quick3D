
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
#include "CLogFloodGate.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define LAT_MAX  90.0

//-------------------------------------------------------------------------------

/*!
    Builds the root of the terrain.
*/
void CWorldTerrain::buildRoot(CRenderContext* pContext)
{
    if (!m_pRoot)
    {
        double startLat = 0.0;
        double startLon = pContext->camera()->geoloc().Longitude;
        startLon = startLon - fmod(startLon, 90.0);
        startLon = Angles::clipAngleDegreePIMinusPI(startLon);

        LOG_DEBUG(QString("CWorldTerrain::buildRoot() : camera lon = %1, start lon = %2").arg(pContext->camera()->geoloc().Longitude).arg(startLon));

        m_pRoot = QSP<CWorldChunk>(new CWorldChunk(m_pScene, this, this));
        m_pRoot->setInheritTransform(false);
        m_pRoot->CComponent::setParent(QSP<CComponent>(this));
        m_pRoot->setGeoloc(CGeoloc(startLat, startLon, 0.0));
        m_pRoot->computeWorldTransform();
        m_pRoot->setSize(CGeoloc(LAT_MAX * 2.0, 360.0, 0.0));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the level of detail in \a iLevel is enough for the chunk in \a pChunk. \br\br
    \a pContext is the rendering context.
*/
bool CWorldTerrain::enoughDetail(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    if (pChunk == nullptr || pContext == nullptr || pContext->camera() == nullptr) return true;

    return enoughDetail(pChunk, pContext->camera()->geoloc(), iLevel);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true when \a iLevel is enough detail for \a pChunk seen from \a gCamera.
*/
bool CWorldTerrain::enoughDetail(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera, int iLevel)
{
    if (pChunk == nullptr) return true;



    return (
                pChunk->worldBounds().containsSpherical(gCamera) == false ||
                iLevel == 0
                )
            && iLevel < m_iLevels;

}

//-------------------------------------------------------------------------------------------------

/*!
    Queues detail generation for \a pChunk once its terrain mesh is ready. \br\br
    The chunk worker never waits for the terrain worker itself: this build
    pass re-queues the chunk as long as its detail is missing, so a chunk
    whose worker ran too early is simply picked up again on a later pass.
*/
void CWorldTerrain::requestChunkDetailWork(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr) return;
    if (pChunk->isOK()) return;
    if (pChunk->terrain() == nullptr || pChunk->terrain()->isOK() == false) return;
    if (pChunk->terrain()->level() >= 2) return;

    CWorkerManager* pManager = CWorkerManager::getInstance();

    if (pManager->containsWorker(pChunk.data()) == false)
    {
        // A chunk whose worker ran before the terrain was ready is marked
        // finished: reset it so the scheduler picks it up again
        pChunk->resetForReuse();
        pManager->addWorker(pChunk.data());
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the terrain and water patches of \a pChunk when missing. \br\br
    Shared by the stay branch and the covering branch of buildRecurse(): both
    need a drawable patch, the stay branch as the final level of detail, the
    covering branch as fallback cover while the finer subdivision finishes.
*/
void CWorldTerrain::ensureChunkTerrain(QSP<CWorldChunk> pChunk, int iLevel)
{
    if (pChunk == nullptr) return;

    // Create terrain for the chunk if needed
    if (pChunk->terrain() == nullptr)
    {
        CGeoloc gOriginalChunkPosition = pChunk->originalGeoloc();
        CGeoloc gOriginalChunkSize = pChunk->originalSize();

        CGeoloc gChunkPosition = pChunk->geoloc();
        CGeoloc gChunkSize = pChunk->size();

        LOG_METHOD_DEBUG(QString("Creating terrain for tile at lat %1, lon %2, level %3")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(iLevel)
                  );

        QSP<CTerrain> pTerrain = QSP<CTerrain>(new CTerrain(
                    m_pScene,
                    m_pHeights,
                    gOriginalChunkPosition,
                    gOriginalChunkSize,
                    gChunkPosition,
                    gChunkSize,
                    m_iTerrainResolution,
                    iLevel,
                    m_iLevels,
                    false,
                    m_bGenerateNow
                    ));

        pTerrain->setMaterial(m_pMaterial);
        pTerrain->setInheritTransform(false);
        pTerrain->setGeoloc(gChunkPosition);
        pTerrain->computeWorldTransform();
        pChunk->setTerrain(pTerrain, m_bGenerateNow);

        if (m_bGenerateNow == false)
        {
            CTiledMaterial* pTiled = dynamic_cast<CTiledMaterial*>(m_pMaterial.data());

            if (pTiled == nullptr)
            {
                QSP<CTerrain> pWater = QSP<CTerrain>(new CTerrain(
                            m_pScene,
                            m_pHeights,
                            gOriginalChunkPosition,
                            gOriginalChunkSize,
                            gChunkPosition,
                            gChunkSize,
                            (int) ((double) m_iTerrainResolution * WaterResolutionFactor),
                            iLevel,
                            m_iLevels,
                            true,
                            m_bGenerateNow
                            ));

                pWater->setInheritTransform(false);
                pWater->setGeoloc(gChunkPosition);
                pWater->computeWorldTransform();
                pChunk->setWater(pWater);
            }
        }

        pChunk->setUsedNow();
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Makes \a pChunk drawable and queues its detail work.
*/
/***************************************************************************/
void CWorldTerrain::ensureChunkCover(QSP<CWorldChunk> pChunk, int iLevel)
{
    ensureChunkTerrain(pChunk, iLevel);

    requestChunkDetailWork(pChunk);
}

//-------------------------------------------------------------------------------------------------

/*!
    Tells whether \a gCamera stands inside the ground extent of \a pChunk. \br\br
    Uses the same worldBounds() primitive as the collect and detail passes, so
    build and collect share one definition of coverage.
*/
bool CWorldTerrain::isCoveringChunk(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera) const
{
    if (pChunk == nullptr) return false;

    return pChunk->worldBounds().containsSpherical(gCamera);
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint() to build necessary terrain patches given the camera location. \br\br
    \a pChunk is any chunk in the chunk tree of the terrain. \br
    \a pContext is the rendering context. \br
    \a iLevel is the depth in the chunk tree.
*/
void CWorldTerrain::buildRecurse(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    if (pChunk == nullptr) return;

    CGeoloc gOriginalChunkPosition = pChunk->originalGeoloc();
    CGeoloc gOriginalChunkSize = pChunk->originalSize();

    CGeoloc gChunkPosition = pChunk->geoloc();

    // Decide whether this level of detail is enough
    bool bStayHere = enoughDetail(pChunk, pContext, iLevel);

    {
        static CLogFloodGate tGate;

        if (tGate.allow() == true)
        {
            LOG_DEBUG(QString("CWorldTerrain::buildRecurse() : level = %1, stay = %2, children = %3, has terrain = %4, cam lat = %5 lon = %6")
                .arg(iLevel).arg(bStayHere).arg(pChunk->childComponents().count()).arg(pChunk->terrain() != nullptr)
                .arg(pContext->camera()->geoloc().Latitude).arg(pContext->camera()->geoloc().Longitude));
        }
    }

    // A partial child set is never valid: subdivision creates four children,
    // removals drop the whole set. Repair here so the recreate block below
    // heals the zone through fresh builds instead of latching on coarse.
    if (CWorldLodPolicy::hasPartialChildren(pChunk->childStatistics().childCount))
    {
        for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);

            destroyChunkSubtree(pChild);

            pChunk->childComponents().remove(iIndex);
            iIndex--;
        }
    }

    if (bStayHere)
    {
        ensureChunkCover(pChunk, iLevel);
    }
    else
    {
        // A chunk covering the camera builds its own patch while the finer
        // subdivision below finishes: the paint recursion then collects the
        // closest ready ancestor instead of leaving a hole. Only the covering
        // column pays this, the far field keeps subdividing without building.
        if (pContext->camera() != nullptr && isCoveringChunk(pChunk, pContext->camera()->geoloc()))
        {
            ensureChunkCover(pChunk, iLevel);
        }

        // Create sub chunks if required
        if (pChunk->childComponents().count() == 0)
        {
            LOG_METHOD_DEBUG(QString("Creating sub-quads for tile at lat %1, lon %2, level %3")
                      .arg(gChunkPosition.Latitude)
                      .arg(gChunkPosition.Longitude)
                      .arg(iLevel)
                      );

            CGeoloc gStart(
                        gOriginalChunkPosition.Latitude - gOriginalChunkSize.Latitude * ChildHalfFraction,
                        gOriginalChunkPosition.Longitude - gOriginalChunkSize.Longitude * ChildHalfFraction,
                        0.0
                        );

            CGeoloc gSize(
                        gOriginalChunkSize.Latitude * ChildHalfFraction,
                        gOriginalChunkSize.Longitude * ChildHalfFraction,
                        0.0
                        );

            {
                // Create for child chunks
                QSP<CWorldChunk> pChild1(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild2(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild3(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild4(new CWorldChunk(m_pScene, this, this));

                // Children don't inherit transforms
                pChild1->setInheritTransform(false);
                pChild2->setInheritTransform(false);
                pChild3->setInheritTransform(false);
                pChild4->setInheritTransform(false);

                // Children's parent are the given chunk
                pChild1->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild2->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild3->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild4->CComponent::setParent(QSP<CComponent>(pChunk));

                // Append the chlidren to the given chunk
                pChunk->childComponents().append(pChild1);
                pChunk->childComponents().append(pChild2);
                pChunk->childComponents().append(pChild3);
                pChunk->childComponents().append(pChild4);

                // Distribute children equally in the given chunk's extents
                pChild1->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * ChildQuarterOffset, gStart.Longitude + gOriginalChunkSize.Longitude * ChildQuarterOffset, 0.0));
                pChild2->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * ChildQuarterOffset, gStart.Longitude + gOriginalChunkSize.Longitude * ChildThreeQuarterOffset, 0.0));
                pChild3->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * ChildThreeQuarterOffset, gStart.Longitude + gOriginalChunkSize.Longitude * ChildQuarterOffset, 0.0));
                pChild4->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * ChildThreeQuarterOffset, gStart.Longitude + gOriginalChunkSize.Longitude * ChildThreeQuarterOffset, 0.0));

                // Compute transforms of children
                pChild1->computeWorldTransform();
                pChild2->computeWorldTransform();
                pChild3->computeWorldTransform();
                pChild4->computeWorldTransform();

                // Set size of children
                pChild1->setSize(gSize);
                pChild2->setSize(gSize);
                pChild3->setSize(gSize);
                pChild4->setSize(gSize);

                // Tell children to build themselves
                pChild1->build();
                pChild2->build();
                pChild3->build();
                pChild4->build();
            }
        }

        // Recurse in child chunks
        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            buildRecurse(pChild, pContext, iLevel - 1);
        }
    }
}

//-------------------------------------------------------------------------------------------------
