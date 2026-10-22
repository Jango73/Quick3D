
// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyInterface.h>

// Application
#include "CLogger.h"
#include "CPhysicsMath.h"
#include "CPhysicsWorld.h"
#include "internal/CPhysicsWorldInternal.h"

//-------------------------------------------------------------------------------------------------

/*!
    Body filter keeping terrain bodies only, height field queries ignore vehicles
*/
class CTerrainBodyFilter final : public JPH::BodyFilter
{

public:

    explicit CTerrainBodyFilter(const QVector<unsigned int>& vTerrainBodies)
        : m_vTerrainBodies(vTerrainBodies)
    {
    }

    bool ShouldCollide(const JPH::BodyID& uBody) const Q_DECL_OVERRIDE
    {
        return m_vTerrainBodies.contains(uBody.GetIndexAndSequenceNumber());
    }

private:

    const QVector<unsigned int>& m_vTerrainBodies;
};

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Casts a ray with explicit Jolt collision filters.
*/
/***************************************************************************/
static bool castFilteredRay(JPH::PhysicsSystem* pSystem, const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM, const JPH::BroadPhaseLayerFilter& cBroadPhaseFilter, const JPH::ObjectLayerFilter& cObjectFilter, const JPH::BodyFilter& cBodyFilter)
{
    if (pSystem == nullptr || dMaxDistanceM <= 0.0 || pHitDistanceM == nullptr)
    {
        return false;
    }

    if (vDirection.magnitude() == 0.0)
    {
        return false;
    }

    Math::CVector3 vNormal = vDirection.normalized();

    JPH::RRayCast cRay(
        CPhysicsJolt::toJolt(vOrigin),
        CPhysicsJolt::toJolt(vNormal) * float(dMaxDistanceM)
        );

    // Note: ground queries hit height field triangles from either side, a
    // ray starting marginally under the surface still reports the ground
    JPH::RayCastSettings cSettings;
    cSettings.SetBackFaceMode(JPH::EBackFaceMode::CollideWithBackFaces);

    JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> cCollector;
    pSystem->GetNarrowPhaseQuery().CastRay(cRay, cSettings, cCollector, cBroadPhaseFilter, cObjectFilter, cBodyFilter);

    if (cCollector.HadHit() == false)
    {
        return false;
    }

    *pHitDistanceM = double(cCollector.mHit.mFraction) * dMaxDistanceM;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a static height field grid, returns the height field id or -1 on failure. \br\br
    \a vHeights holds \a iSampleCount rows of \a iSampleCount samples in meters, row 0 is the north edge, column 0 the west edge. \br
    \a vPosition is the grid center at zero datum, the local frame has +X east, +Y up and +Z north.
*/
int CPhysicsWorld::addHeightfield(const QVector<float>& vHeights, int iSampleCount, float fSpacingXM, float fSpacingZM, const Math::CVector3& vPosition, const Math::CMatrix4& mRotation)
{
    if (isReady() == false || iSampleCount < 2 || fSpacingXM <= 0.0f || fSpacingZM <= 0.0f)
    {
        return -1;
    }

    if (vHeights.count() != iSampleCount * iSampleCount)
    {
        return -1;
    }

    // Note: Jolt numbers rows from +Z, +Z points north here, the engine grid
    // numbers rows from the north edge, rows are flipped on copy so both
    // row 0 sit on the north edge
    JPH::Array<float> vSamples;
    vSamples.reserve(vHeights.count());

    for (int iRow = 0; iRow < iSampleCount; iRow++)
    {
        for (int iCol = 0; iCol < iSampleCount; iCol++)
        {
            vSamples.push_back(vHeights[(iSampleCount - 1 - iRow) * iSampleCount + iCol]);
        }
    }

    float fHalfXM = 0.5f * float(iSampleCount - 1) * fSpacingXM;
    float fHalfZM = 0.5f * float(iSampleCount - 1) * fSpacingZM;

    // Note: the shape quantizes samples over the value range, a flat grid
    // has a null range, pad it so the quantization scale stays valid
    float fMinimumM = vSamples[0];
    float fMaximumM = vSamples[0];

    foreach (float fSampleM, vSamples)
    {
        if (fSampleM < fMinimumM) fMinimumM = fSampleM;
        if (fSampleM > fMaximumM) fMaximumM = fSampleM;
    }

    if (fMaximumM - fMinimumM < 1.0e-3f)
    {
        fMinimumM -= 0.5f;
        fMaximumM += 0.5f;
    }

    JPH::HeightFieldShapeSettings cSettings(
        vSamples.data(),
        JPH::Vec3(-fHalfXM, 0.0f, -fHalfZM),
        JPH::Vec3(fSpacingXM, 1.0f, fSpacingZM),
        JPH::uint32(iSampleCount)
        );

    cSettings.mMinHeightValue = fMinimumM;
    cSettings.mMaxHeightValue = fMaximumM;

    JPH::Shape::ShapeResult cShapeResult = cSettings.Create();
    JPH::ShapeRefC pShape = cShapeResult.Get();

    if (isValidShape(cShapeResult, pShape) == false)
    {
        LOG_METHOD_WARNING("Height field shape creation failed");
        return -1;
    }

    JPH::Body* pBody = createBody(
        m_pPhysicsSystem,
        pShape,
        CPhysicsJolt::toJolt(vPosition),
        CPhysicsJolt::toJoltRotation(CPhysicsMath::toQuaternion(mRotation)),
        JPH::EMotionType::Static,
        CPhysicsLayers::NON_MOVING,
        1.0f
        );

    if (pBody == nullptr)
    {
        LOG_METHOD_WARNING("Height field body creation failed");
        return -1;
    }

    m_pPhysicsSystem->GetBodyInterface().AddBody(pBody->GetID(), JPH::EActivation::DontActivate);

    CHeightfieldRecord cRecord;
    cRecord.m_uBody = pBody->GetID().GetIndexAndSequenceNumber();

    int iHeightfield = m_iNextHeightfieldId;
    m_iNextHeightfieldId++;

    m_mHeightfields.insert(iHeightfield, cRecord);

    return iHeightfield;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes a height field, returns false for an unknown height field id
*/
bool CPhysicsWorld::removeHeightfield(int iHeightfield)
{
    if (isReady() == false || m_mHeightfields.contains(iHeightfield) == false)
    {
        return false;
    }

    JPH::BodyInterface& cBodies = m_pPhysicsSystem->GetBodyInterface();
    cBodies.RemoveBody(JPH::BodyID(m_mHeightfields[iHeightfield].m_uBody));
    cBodies.DestroyBody(JPH::BodyID(m_mHeightfields[iHeightfield].m_uBody));

    m_mHeightfields.remove(iHeightfield);

    // Note: Jolt never wakes a sleeper when its support vanishes, without
    // activation a parked chassis floats forever over the new ground
    activateVehicles();

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all height fields
*/
void CPhysicsWorld::clearHeightfields()
{
    // Note: keys are copied, removal during iteration is safe
    foreach (int iHeightfield, m_mHeightfields.keys())
    {
        removeHeightfield(iHeightfield);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Casts a ray against all bodies, fills \a pHitDistanceM on hit. \br\br
    \a vDirection is normalized inside, \a dMaxDistanceM bounds the search.
*/
bool CPhysicsWorld::castRay(const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM)
{
    if (isReady() == false)
    {
        return false;
    }

    return castFilteredRay(m_pPhysicsSystem, vOrigin, vDirection, dMaxDistanceM, pHitDistanceM, JPH::BroadPhaseLayerFilter(), JPH::ObjectLayerFilter(), JPH::BodyFilter());
}

//-------------------------------------------------------------------------------------------------

/*!
    Casts a ray against registered height fields only, fills \a pHitDistanceM on hit. \br\br
    Vehicles and other static bodies are ignored, use it for terrain ground queries.
*/
bool CPhysicsWorld::castTerrainRay(const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM)
{
    if (isReady() == false)
    {
        return false;
    }

    QVector<unsigned int> vTerrainBodies;
    vTerrainBodies.reserve(m_mHeightfields.count());

    foreach (const CHeightfieldRecord& cRecord, m_mHeightfields)
    {
        vTerrainBodies.append(cRecord.m_uBody);
    }

    CTerrainBodyFilter cTerrainFilter(vTerrainBodies);
    JPH::SpecifiedBroadPhaseLayerFilter cBroadPhaseFilter(CPhysicsBroadPhaseLayers::NON_MOVING);
    JPH::SpecifiedObjectLayerFilter cObjectFilter(CPhysicsLayers::NON_MOVING);

    return castFilteredRay(m_pPhysicsSystem, vOrigin, vDirection, dMaxDistanceM, pHitDistanceM, cBroadPhaseFilter, cObjectFilter, cTerrainFilter);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the number of height fields in the world
*/
unsigned int CPhysicsWorld::heightfieldCount() const
{
    return m_mHeightfields.count();
}

//-------------------------------------------------------------------------------------------------
