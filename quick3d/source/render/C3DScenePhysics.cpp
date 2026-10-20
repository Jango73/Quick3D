
// qt-plus
#include "CLogger.h"

// STL
#include <cmath>

// Application
#include "C3DScene.h"
#include "CVehicle.h"
#include "CPhysicsMath.h"
#include "CAxis.h"

//-------------------------------------------------------------------------------------------------

namespace
{
    const double PHYSICS_STEP_S = 1.0 / 60.0;
    const int PHYSICS_MAX_STEPS_PER_FRAME = 4;
    const double PHYSICS_GROUND_RAY_HEIGHT_M = 50.0;
    const double PHYSICS_GROUND_RAY_LENGTH_M = 200.0;
    const double PHYSICS_GROUND_SNAP_ABOVE_M = 20.0;
}

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Registers a wheeled vehicle in the physics world, returns false on failure. \br\br
    Freezes the physics reference on first attach, gravity follows the vehicle
    down direction. The reference stays frozen while bodies are attached, moving
    it requires detaching all vehicles first (rebase is future work).
*/
bool C3DScene::attachVehicleToPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr || pVehicle->isRootObject() == false)
    {
        return false;
    }

    if (pVehicle->hasWheels() == false)
    {
        LOG_METHOD_WARNING("Vehicle has no wheels");
        return false;
    }

    CVector3 vHalfExtentsM = pVehicle->chassisHalfExtentsM();

    if (vHalfExtentsM.X <= 0.0 || vHalfExtentsM.Y <= 0.0 || vHalfExtentsM.Z <= 0.0)
    {
        LOG_METHOD_WARNING("Vehicle has no chassis bounds");
        return false;
    }

    QVector<CVehicleWheel> vPhysicsWheels = pVehicle->buildPhysicsWheels();

    // Note: visual bounds overhang the wheels (wide armored hulls), a wide
    // collision box strikes the ground on roll and kicks the chassis from
    // side to side, clamp the footprint to the wheel track plus a margin
    double dMaxWheelXM = 0.0;
    double dMaxWheelZM = 0.0;

    foreach (const CVehicleWheel& cWheel, vPhysicsWheels)
    {
        if (fabs(cWheel.m_vPosition.X) > dMaxWheelXM)
        {
            dMaxWheelXM = fabs(cWheel.m_vPosition.X);
        }

        if (fabs(cWheel.m_vPosition.Z) > dMaxWheelZM)
        {
            dMaxWheelZM = fabs(cWheel.m_vPosition.Z);
        }
    }

    CVector3 vCenterOffsetM = pVehicle->chassisCenterOffsetM();
    vHalfExtentsM.X = qMin(vHalfExtentsM.X, dMaxWheelXM + fabs(vCenterOffsetM.X) + 0.15);
    vHalfExtentsM.Z = qMin(vHalfExtentsM.Z, dMaxWheelZM + fabs(vCenterOffsetM.Z) + 0.15);

    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return false;
    }

    if (m_bPhysicsReferenceValid == false)
    {
        ensurePhysicsReference(pVehicle->geoloc());
    }

    // Note: the reference can be frozen far from gameplay, gravity and the
    // spawn lift follow the vehicle up direction, never that distant point.
    CVector3 vVehicleUpM = pVehicle->geoloc().getTopocentricAxis().Up;
    pWorld->setGravity(vVehicleUpM * -9.81);

    CVector3 vReferenceECEF = m_gPhysicsReference.toVector3();
    CVector3 vLocalPositionM = pVehicle->geoloc().toVector3() - vReferenceECEF;

    // Note: authored axles sit low for the arcade pose, lift the spawn so
    // tires touch at mid suspension travel instead of ejecting the chassis.
    // The pose stays level by construction, lifting along the reference up.
    double dLiftM = 0.0;

    foreach (const CVehicleWheel& cWheel, vPhysicsWheels)
    {
        double dWheelLiftM = cWheel.m_dRadiusM + 0.5 * (cWheel.m_dSuspensionMinLengthM + cWheel.m_dSuspensionMaxLengthM) - cWheel.m_vPosition.Y;

        if (dWheelLiftM > dLiftM)
        {
            dLiftM = dWheelLiftM;
        }
    }

    if (dLiftM > 0.0)
    {
        vLocalPositionM = vLocalPositionM + vVehicleUpM * dLiftM;
    }

    CQuaternion cRotation = CPhysicsMath::toQuaternion(CMatrix4::makeRotation(pVehicle->ECEFRotation()));

    return pVehicle->attachPhysicsVehicle(pWorld, vHalfExtentsM, vLocalPositionM, cRotation, pVehicle->chassisCenterOffsetM());
}

//-------------------------------------------------------------------------------------------------

/*!
    Freezes the physics reference on \a gReference when none is set. \br\br
    The first frozen reference wins, later calls are ignored.
*/
void C3DScene::ensurePhysicsReference(const CGeoloc& gReference)
{
    if (m_bPhysicsReferenceValid == true)
    {
        return;
    }

    m_gPhysicsReference = gReference;
    m_bPhysicsReferenceValid = true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes a vehicle from the physics world, does nothing when detached
*/
void C3DScene::detachVehicleFromPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr)
    {
        return;
    }

    pVehicle->detachPhysicsVehicle();
}

//-------------------------------------------------------------------------------------------------

/*!
    Attaches grounded root vehicles to the physics world. \br\br
    A candidate needs active physics, wheels and no attached body yet. A
    registered height field must reach it first: a downward terrain ray in
    the physics frame decides, the arcade path keeps unattached vehicles
    grounded meanwhile. A close spawn snaps to the measured ground before
    the body is created.
*/
void C3DScene::ensureVehiclePhysics()
{
    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    foreach (QSP<CVehicle> pVehicle, m_vSceneVehicles)
    {
        if (pVehicle == nullptr || pVehicle->isRootObject() == false)
        {
            continue;
        }

        if (pVehicle->physicsActive() == false || pVehicle->hasPhysicsVehicle() == true)
        {
            continue;
        }

        if (pVehicle->hasWheels() == false)
        {
            continue;
        }

        ensurePhysicsReference(pVehicle->geoloc());

        CGeoloc gVehicle = pVehicle->geoloc();
        CVector3 vUp = gVehicle.getTopocentricAxis().Up;

        CGeoloc gRayOrigin(gVehicle.Latitude, gVehicle.Longitude, gVehicle.Altitude + PHYSICS_GROUND_RAY_HEIGHT_M);
        CVector3 vOrigin = gRayOrigin.toVector3() - m_gPhysicsReference.toVector3();

        double dHitDistanceM = 0.0;

        if (pWorld->castTerrainRay(vOrigin, vUp * -1.0, PHYSICS_GROUND_RAY_LENGTH_M, &dHitDistanceM) == false)
        {
            continue;
        }

        CGeoloc gGround(m_gPhysicsReference.toVector3() + vOrigin - vUp * dHitDistanceM);

        // Note: only a close spawn moves to the measured ground, a high
        // spawn keeps its authored altitude instead of teleporting down.
        if (gVehicle.Altitude < gGround.Altitude + PHYSICS_GROUND_SNAP_ABOVE_M)
        {
            pVehicle->setGeoloc(gGround);
        }

        attachVehicleToPhysics(pVehicle);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Advances the physics world at a fixed step and syncs attached vehicles. \br\br
    Caps the steps per frame, excess time is dropped to avoid spiral of death.
*/
void C3DScene::stepPhysics(double dDeltaTimeS)
{
    if (m_bPhysicsInitialized == false)
    {
        return;
    }

    QVector<QSP<CVehicle> > vAttachedVehicles;

    foreach (QSP<CVehicle> pVehicle, m_vSceneVehicles)
    {
        if (pVehicle != nullptr && pVehicle->hasPhysicsVehicle() == true)
        {
            vAttachedVehicles.append(pVehicle);
        }
    }

    if (vAttachedVehicles.count() == 0)
    {
        m_dPhysicsAccumulatorS = 0.0;
        return;
    }

    if (dDeltaTimeS > 0.0)
    {
        m_dPhysicsAccumulatorS += dDeltaTimeS;
    }

    int iSteps = 0;

    while (m_dPhysicsAccumulatorS >= PHYSICS_STEP_S && iSteps < PHYSICS_MAX_STEPS_PER_FRAME)
    {
        m_PhysicsWorld.step(PHYSICS_STEP_S);
        m_dPhysicsAccumulatorS -= PHYSICS_STEP_S;
        m_uPhysicsSteps++;
        iSteps++;
    }

    if (m_dPhysicsAccumulatorS >= PHYSICS_STEP_S)
    {
        m_dPhysicsAccumulatorS = 0.0;
    }

    foreach (QSP<CVehicle> pVehicle, vAttachedVehicles)
    {
        syncVehicleFromPhysics(pVehicle);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Writes the physics chassis pose back to the vehicle geoloc and rotation. \br\br
    The physics frame is the ECEF frame translated to the frozen reference,
    velocities are projected on the reference topocentric axes.
*/
void C3DScene::syncVehicleFromPhysics(QSP<CVehicle> pVehicle)
{
    if (pVehicle == nullptr || pVehicle->hasPhysicsVehicle() == false)
    {
        return;
    }

    CPhysicsWorld* pWorld = physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    int iVehicle = pVehicle->physicsVehicleIndex();

    CVector3 vBodyPosition;
    CQuaternion cBodyRotation;
    CVector3 vLinearVelocity;
    CVector3 vAngularVelocity;
    CVector3 vCenterOffsetM;

    if (pWorld->vehiclePosition(iVehicle, vBodyPosition) == false)
    {
        return;
    }

    if (pWorld->vehicleRotation(iVehicle, cBodyRotation) == false)
    {
        return;
    }

    pWorld->vehicleLinearVelocity(iVehicle, vLinearVelocity);
    pWorld->vehicleAngularVelocity(iVehicle, vAngularVelocity);
    pWorld->vehicleCenterOffsetM(iVehicle, vCenterOffsetM);

    CMatrix4 mBodyRotation = CMatrix4::fromQuaternion(cBodyRotation);

    CVector3 vOriginECEF = vBodyPosition + m_gPhysicsReference.toVector3() - (mBodyRotation * vCenterOffsetM);
    CGeoloc gOrigin(vOriginECEF);

    pVehicle->setGeoloc(gOrigin);

    CAxis aBody;
    aBody.Right = CVector3(mBodyRotation.Data[0][0], mBodyRotation.Data[0][1], mBodyRotation.Data[0][2]);
    aBody.Up = CVector3(mBodyRotation.Data[1][0], mBodyRotation.Data[1][1], mBodyRotation.Data[1][2]);
    aBody.Front = CVector3(mBodyRotation.Data[2][0], mBodyRotation.Data[2][1], mBodyRotation.Data[2][2]);

    CVector3 vLocalRotation = CAxis(aBody.eulerAngles()).transferFrom(gOrigin.getTopocentricAxis()).eulerAngles();

    pVehicle->setRotation(vLocalRotation);

    CAxis aReference = m_gPhysicsReference.getTopocentricAxis();

    pVehicle->setVelocity_ms(CVector3(vLinearVelocity.dot(aReference.Right), vLinearVelocity.dot(aReference.Up), vLinearVelocity.dot(aReference.Front)));
    pVehicle->setAngularVelocity_rs(CVector3(vAngularVelocity.dot(aReference.Right), vAngularVelocity.dot(aReference.Up), vAngularVelocity.dot(aReference.Front)));
}

//-------------------------------------------------------------------------------------------------

/*!
    Rebuilds the vehicle registry from the scene components. \br\br
    Runs at registration time only, per frame code iterates the registry
    without type queries.
*/
void C3DScene::rebuildVehicleRegistry()
{
    m_vSceneVehicles.clear();

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        registerVehicleComponent(pComponent);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers \a pComponent in the vehicle registry when it is a vehicle
*/
void C3DScene::registerVehicleComponent(QSP<CComponent> pComponent)
{
    QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

    if (pVehicle == nullptr)
    {
        return;
    }

    if (m_vSceneVehicles.contains(pVehicle) == false)
    {
        m_vSceneVehicles.append(pVehicle);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes \a pComponent from the vehicle registry, does nothing otherwise
*/
void C3DScene::unregisterVehicleComponent(QSP<CComponent> pComponent)
{
    QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

    if (pVehicle == nullptr)
    {
        return;
    }

    m_vSceneVehicles.removeAll(pVehicle);
}

//-------------------------------------------------------------------------------------------------
