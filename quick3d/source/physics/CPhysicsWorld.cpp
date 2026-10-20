
// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyInterface.h>

// Qt
#include <QThread>
#include <QMutex>

// Application
#include "CLogger.h"
#include "CPhysicsWorld.h"
#include "CPhysicsMath.h"

//-------------------------------------------------------------------------------------------------

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer COUNT = 2;
}

//-------------------------------------------------------------------------------------------------

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr unsigned int COUNT = 2;
}

//-------------------------------------------------------------------------------------------------

namespace VehicleTorque
{
    // Note: Jolt vehicle defaults target a 1500 kg reference car with
    // 0.3 m wheels, torque scales with chassis mass and compensates the
    // largest tire radius so wheel force per ton stays constant, grade
    // torque comes from the truck first gear, top speed from the controller
    // speed limiter, never from oversized torque
    static constexpr double REFERENCE_MASS_KG = 1500.0;
    static constexpr double ENGINE_TORQUE_NM = 250.0;
    static constexpr double BRAKE_TORQUE_NM = 1500.0;
    static constexpr double HANDBRAKE_TORQUE_NM = 4000.0;
    // Note: Jolt stock engine and transmission defaults, the scaled engine
    // keeps their ratios so heavy vehicles couple and rev like the car
    static constexpr double REFERENCE_ENGINE_TORQUE_NM = 500.0;
    static constexpr double REFERENCE_CLUTCH_STRENGTH = 10.0;
    static constexpr double REFERENCE_ENGINE_INERTIA = 0.5;
    // Note: Jolt sample anti roll bar is 1000 N/m for the 1500 kg reference
    // car, stiffness scales linearly so heavy vehicles resist roll without
    // coupling the axles into a rigid bouncing beam
    static constexpr double ANTI_ROLLBAR_REFERENCE_N_PER_M = 1000.0;
    // Note: maximum pitch and roll before the constraint pulls the chassis
    // back, military vehicles climb slopes but never flip onto the roof
    static constexpr double MAX_PITCH_ROLL_ANGLE_RAD = 1.0471975512;
}

//-------------------------------------------------------------------------------------------------

class CBroadPhaseLayerMapper final : public JPH::BroadPhaseLayerInterface
{

public:

    unsigned int GetNumBroadPhaseLayers() const Q_DECL_OVERRIDE
    {
        return BroadPhaseLayers::COUNT;
    }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer iLayer) const Q_DECL_OVERRIDE
    {
        if (iLayer == Layers::NON_MOVING)
        {
            return BroadPhaseLayers::NON_MOVING;
        }

        return BroadPhaseLayers::MOVING;
    }

#if defined(JPH_PROFILE_ENABLED) || defined(JPH_EXTERNAL_PROFILE)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer iLayer) const Q_DECL_OVERRIDE
    {
        if (iLayer == BroadPhaseLayers::NON_MOVING)
        {
            return "NON_MOVING";
        }

        return "MOVING";
    }
#endif
};

//-------------------------------------------------------------------------------------------------

class CObjectBroadPhaseFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
{

public:

    bool ShouldCollide(JPH::ObjectLayer iObject, JPH::BroadPhaseLayer iBroadPhase) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(iBroadPhase)

        if (iObject == Layers::NON_MOVING)
        {
            return false;
        }

        return true;
    }
};

//-------------------------------------------------------------------------------------------------

class CObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter
{

public:

    bool ShouldCollide(JPH::ObjectLayer iFirst, JPH::ObjectLayer iSecond) const Q_DECL_OVERRIDE
    {
        if (iFirst == Layers::NON_MOVING)
        {
            return iSecond == Layers::MOVING;
        }

        return true;
    }
};

//-------------------------------------------------------------------------------------------------

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

static QMutex& physicsInitMutex()
{
    static QMutex s_Mutex;

    return s_Mutex;
}

//-------------------------------------------------------------------------------------------------

static int& physicsInitCount()
{
    static int s_iCount = 0;

    return s_iCount;
}

//-------------------------------------------------------------------------------------------------

static CBroadPhaseLayerMapper& broadPhaseLayerMapper()
{
    static CBroadPhaseLayerMapper s_Mapper;

    return s_Mapper;
}

//-------------------------------------------------------------------------------------------------

static CObjectBroadPhaseFilter& objectBroadPhaseFilter()
{
    static CObjectBroadPhaseFilter s_Filter;

    return s_Filter;
}

//-------------------------------------------------------------------------------------------------

static CObjectLayerPairFilter& objectLayerPairFilter()
{
    static CObjectLayerPairFilter s_Filter;

    return s_Filter;
}

//-------------------------------------------------------------------------------------------------

static void joltTrace(const char* pFormat, ...)
{
    char sBuffer[1024];

    va_list vArgs;
    va_start(vArgs, pFormat);
    vsnprintf(sBuffer, sizeof(sBuffer), pFormat, vArgs);
    va_end(vArgs);

    LOG_DEBUG(QString::fromUtf8(sBuffer));
}

//-------------------------------------------------------------------------------------------------

#ifdef JPH_ENABLE_ASSERTS
static bool joltAssertFailed(const char* pExpression, const char* pMessage, const char* pFile, unsigned int uLine)
{
    LOG_ERROR(QString("Jolt assert %1 %2 (%3:%4)").arg(pExpression).arg(pMessage).arg(pFile).arg(uLine));

    return true;
}
#endif

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
        CPhysicsMath::toJolt(vOrigin),
        CPhysicsMath::toJolt(vNormal) * float(dMaxDistanceM)
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
    Builds an uninitialized world, call initialize() before use
*/
CPhysicsWorld::CPhysicsWorld()
    : m_bInitialized(false)
    , m_pPhysicsSystem(nullptr)
    , m_pJobSystem(nullptr)
    , m_pTempAllocator(nullptr)
    , m_iNextHeightfieldId(0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Releases Jolt resources
*/
CPhysicsWorld::~CPhysicsWorld()
{
    shutdown();
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the physics system and its job pool, returns false on failure
*/
bool CPhysicsWorld::initialize()
{
    QMutexLocker cLocker(&physicsInitMutex());

    if (m_bInitialized == true)
    {
        return true;
    }

    if (physicsInitCount() == 0)
    {
        JPH::RegisterDefaultAllocator();
        JPH::Trace = joltTrace;
#ifdef JPH_ENABLE_ASSERTS
        JPH::AssertFailed = joltAssertFailed;
#endif
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
    }

    physicsInitCount()++;

    int iThreads = QThread::idealThreadCount() - 1;

    if (iThreads < 1)
    {
        iThreads = 1;
    }

    if (iThreads > 8)
    {
        iThreads = 8;
    }

    m_pJobSystem = new JPH::JobSystemThreadPool(256, 8, iThreads);
    m_pTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    m_pPhysicsSystem = new JPH::PhysicsSystem();

    m_pPhysicsSystem->Init(1024, 0, 1024, 1024, broadPhaseLayerMapper(), objectBroadPhaseFilter(), objectLayerPairFilter());
    m_pPhysicsSystem->OptimizeBroadPhase();

    m_bInitialized = true;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the physics system and its job pool
*/
void CPhysicsWorld::shutdown()
{
    QMutexLocker cLocker(&physicsInitMutex());

    if (m_bInitialized == false)
    {
        return;
    }

    // Note: ConstraintManager stores constraints as Ref<Constraint> and releases
    // its reference on RemoveConstraint, which deletes the constraint. No
    // manual delete here, it would free the object twice.
    for (int iVehicle = 0; iVehicle < m_vVehicles.count(); iVehicle++)
    {
        m_pPhysicsSystem->RemoveStepListener(m_vVehicles[iVehicle].m_pConstraint);
        m_pPhysicsSystem->RemoveConstraint(m_vVehicles[iVehicle].m_pConstraint);
    }

    m_vVehicles.clear();

    delete m_pPhysicsSystem;
    delete m_pJobSystem;
    delete m_pTempAllocator;

    m_pPhysicsSystem = nullptr;
    m_pJobSystem = nullptr;
    m_pTempAllocator = nullptr;
    m_vBodies.clear();
    m_bInitialized = false;

    physicsInitCount()--;

    if (physicsInitCount() == 0)
    {
        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Advances the simulation by the given step in seconds, fixed step expected
*/
void CPhysicsWorld::step(double dDeltaTimeSeconds)
{
    if (m_bInitialized == false || dDeltaTimeSeconds <= 0.0)
    {
        return;
    }

    m_pPhysicsSystem->Update(float(dDeltaTimeSeconds), 1, m_pTempAllocator, m_pJobSystem);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the gravity vector in meters per second per second, default is (0, -9.81, 0)
*/
void CPhysicsWorld::setGravity(const Math::CVector3& vGravity)
{
    if (m_bInitialized == false)
    {
        return;
    }

    m_pPhysicsSystem->SetGravity(CPhysicsMath::toJolt(vGravity));
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the current gravity vector in meters per second per second
*/
Math::CVector3 CPhysicsWorld::gravity() const
{
    if (m_bInitialized == false || m_pPhysicsSystem == nullptr)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    return CPhysicsMath::fromJolt(m_pPhysicsSystem->GetGravity());
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a static box, returns the body index or -1 on failure
*/
int CPhysicsWorld::addStaticBox(const Math::CVector3& vHalfExtents, const Math::CVector3& vPosition)
{
    if (m_bInitialized == false)
    {
        LOG_METHOD_ERROR("World not initialized");
        return -1;
    }

    JPH::BoxShapeSettings cShapeSettings(CPhysicsMath::toJolt(vHalfExtents));
    JPH::Shape::ShapeResult cShapeResult = cShapeSettings.Create();
    JPH::ShapeRefC pShape = cShapeResult.Get();

    if (cShapeResult.IsValid() == false || pShape == nullptr)
    {
        LOG_METHOD_ERROR("Box shape creation failed");
        return -1;
    }

    JPH::BodyCreationSettings cSettings(pShape, CPhysicsMath::toJolt(vPosition), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
    JPH::BodyID iBodyID = m_pPhysicsSystem->GetBodyInterface().CreateAndAddBody(cSettings, JPH::EActivation::DontActivate);

    if (iBodyID.IsInvalid() == true)
    {
        LOG_METHOD_ERROR("Box body creation failed");
        return -1;
    }

    m_vBodies.append(iBodyID.GetIndexAndSequenceNumber());

    return m_vBodies.count() - 1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a dynamic sphere, returns the body index or -1 on failure
*/
int CPhysicsWorld::addDynamicSphere(double dRadius, const Math::CVector3& vPosition)
{
    if (m_bInitialized == false)
    {
        LOG_METHOD_ERROR("World not initialized");
        return -1;
    }

    if (dRadius <= 0.0)
    {
        LOG_METHOD_ERROR("Invalid sphere radius");
        return -1;
    }

    JPH::SphereShapeSettings cShapeSettings{float(dRadius)};
    JPH::Shape::ShapeResult cShapeResult = cShapeSettings.Create();
    JPH::ShapeRefC pShape = cShapeResult.Get();

    if (cShapeResult.IsValid() == false || pShape == nullptr)
    {
        LOG_METHOD_ERROR("Sphere shape creation failed");
        return -1;
    }

    JPH::BodyCreationSettings cSettings(pShape, CPhysicsMath::toJolt(vPosition), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    JPH::BodyID iBodyID = m_pPhysicsSystem->GetBodyInterface().CreateAndAddBody(cSettings, JPH::EActivation::Activate);

    if (iBodyID.IsInvalid() == true)
    {
        LOG_METHOD_ERROR("Sphere body creation failed");
        return -1;
    }

    m_vBodies.append(iBodyID.GetIndexAndSequenceNumber());

    return m_vBodies.count() - 1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a body position, returns false for an unknown body index
*/
bool CPhysicsWorld::bodyPosition(int iBody, Math::CVector3& vPosition) const
{
    if (m_bInitialized == false || iBody < 0 || iBody >= m_vBodies.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vBodies[iBody]));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    vPosition = CPhysicsMath::fromJolt(cLock.GetBody().GetPosition());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the number of bodies in the world
*/
unsigned int CPhysicsWorld::bodyCount() const
{
    if (m_bInitialized == false)
    {
        return 0;
    }

    return m_pPhysicsSystem->GetNumBodies();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Applies the Jolt tire grip shape scaled by the authored level.
*/
static void applyTireFriction(JPH::LinearCurve& cCurve, bool bLongitudinal, double dFrictionNorm)
{
    cCurve.Clear();

    if (bLongitudinal == true)
    {
        // Note: grip at zero slip holds slopes and launches at crawl speed,
        // a zero intercept only bites after spinning up, which the gearbox
        // reads as speed and answers with an upshift and a torque cut
        cCurve.AddPoint(0.0f, float(1.0 * dFrictionNorm));
        cCurve.AddPoint(0.06f, float(1.2 * dFrictionNorm));
        cCurve.AddPoint(0.2f, float(1.0 * dFrictionNorm));
    }
    else
    {
        cCurve.AddPoint(0.0f, 0.0f);
        cCurve.AddPoint(3.0f, float(1.2 * dFrictionNorm));
        cCurve.AddPoint(20.0f, float(1.0 * dFrictionNorm));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a dynamic chassis with a VehicleConstraint, returns the vehicle index or -1 on failure. \br\br
    \a vPosition is the vehicle origin, the body spawns shifted by \a vCenterOffsetM rotated into the chassis frame.
*/
int CPhysicsWorld::addVehicle(const Math::CVector3& vChassisHalfExtentsM, double dChassisMassKg, const Math::CVector3& vPosition, const QVector<CVehicleWheel>& vWheels, const Math::CQuaternion& cRotation, const Math::CVector3& vCenterOffsetM)
{
    if (m_bInitialized == false)
    {
        LOG_METHOD_ERROR("World not initialized");
        return -1;
    }

    if (vChassisHalfExtentsM.X <= 0.0 || vChassisHalfExtentsM.Y <= 0.0 || vChassisHalfExtentsM.Z <= 0.0 || dChassisMassKg <= 0.0)
    {
        LOG_METHOD_ERROR("Invalid chassis dimensions or mass");
        return -1;
    }

    if (vWheels.count() < 2)
    {
        LOG_METHOD_ERROR("A vehicle needs at least two wheels");
        return -1;
    }

    QVector<int> vDrivenWheels;

    for (int iWheel = 0; iWheel < vWheels.count(); iWheel++)
    {
        const CVehicleWheel& cWheel = vWheels[iWheel];

        if (cWheel.m_dRadiusM <= 0.0 || cWheel.m_dWidthM <= 0.0 || cWheel.m_dSuspensionMinLengthM < 0.0 || cWheel.m_dSuspensionMaxLengthM <= cWheel.m_dSuspensionMinLengthM)
        {
            LOG_METHOD_ERROR(QString("Invalid wheel settings at index %1").arg(iWheel));
            return -1;
        }

        if (cWheel.m_bDriven == true)
        {
            vDrivenWheels.append(iWheel);
        }
    }

    if (vDrivenWheels.count() % 2 != 0)
    {
        LOG_METHOD_ERROR("Driven wheels pair into differentials, their count must be even");
        return -1;
    }

    // Note: the fresh BoxShape pointer is adopted by the settings object, passing
    // the raw pointer out of a ShapeResult would dangle (DecoratedShapeSettings
    // stores it without reference counting)
    JPH::Vec3 vHalfExtents = CPhysicsMath::toJolt(vChassisHalfExtentsM);
    JPH::RefConst<JPH::Shape> pChassisShape = JPH::OffsetCenterOfMassShapeSettings(JPH::Vec3(0.0f, -vHalfExtents.GetY(), 0.0f), new JPH::BoxShape(vHalfExtents)).Create().Get();

    if (pChassisShape == nullptr)
    {
        LOG_METHOD_ERROR("Chassis shape creation failed");
        return -1;
    }

    JPH::Quat cChassisRotation = CPhysicsMath::toJoltRotation(cRotation).Normalized();
    JPH::Vec3 vBodyPosition = CPhysicsMath::toJolt(vPosition) + cChassisRotation * CPhysicsMath::toJolt(vCenterOffsetM);

    JPH::BodyCreationSettings cBodySettings(pChassisShape, vBodyPosition, cChassisRotation, JPH::EMotionType::Dynamic, Layers::MOVING);
    cBodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
    cBodySettings.mMassPropertiesOverride.mMass = float(dChassisMassKg);

    JPH::Body* pChassisBody = m_pPhysicsSystem->GetBodyInterface().CreateBody(cBodySettings);

    if (pChassisBody == nullptr)
    {
        LOG_METHOD_ERROR("Chassis body creation failed");
        return -1;
    }

    m_pPhysicsSystem->GetBodyInterface().AddBody(pChassisBody->GetID(), JPH::EActivation::Activate);

    JPH::VehicleConstraintSettings cVehicleSettings;

    for (int iWheel = 0; iWheel < vWheels.count(); iWheel++)
    {
        const CVehicleWheel& cWheel = vWheels[iWheel];

        JPH::WheelSettingsWV* pWheelSettings = new JPH::WheelSettingsWV();

        // Note: wheel positions arrive in the vehicle origin frame, the
        // constraint expects them in the body center frame
        pWheelSettings->mPosition = CPhysicsMath::toJolt(cWheel.m_vPosition - vCenterOffsetM);
        pWheelSettings->mSuspensionMinLength = float(cWheel.m_dSuspensionMinLengthM);
        pWheelSettings->mSuspensionMaxLength = float(cWheel.m_dSuspensionMaxLengthM);
        pWheelSettings->mSuspensionSpring.mFrequency = float(cWheel.m_dSuspensionFrequencyHz);
        pWheelSettings->mSuspensionSpring.mDamping = float(cWheel.m_dSuspensionDampingNorm);
        pWheelSettings->mRadius = float(cWheel.m_dRadiusM);
        pWheelSettings->mWidth = float(cWheel.m_dWidthM);
        pWheelSettings->mInertia = 0.5f * 20.0f * float(cWheel.m_dRadiusM) * float(cWheel.m_dRadiusM);
        pWheelSettings->mMaxSteerAngle = float(cWheel.m_dMaxSteerAngleRad);
        // Note: flat friction at zero slip locks the tires at standstill and
        // shakes the chassis, keep the Jolt progressive tire shape instead
        applyTireFriction(pWheelSettings->mLongitudinalFriction, true, cWheel.m_dLongitudinalFrictionNorm);
        applyTireFriction(pWheelSettings->mLateralFriction, false, cWheel.m_dLateralFrictionNorm);
        pWheelSettings->mMaxBrakeTorque = float(dChassisMassKg * VehicleTorque::BRAKE_TORQUE_NM / VehicleTorque::REFERENCE_MASS_KG);

        if (cWheel.m_bHandBrake == false)
        {
            pWheelSettings->mMaxHandBrakeTorque = 0.0f;
        }
        else
        {
            pWheelSettings->mMaxHandBrakeTorque = float(dChassisMassKg * VehicleTorque::HANDBRAKE_TORQUE_NM / VehicleTorque::REFERENCE_MASS_KG);
        }

        cVehicleSettings.mWheels.push_back(pWheelSettings);
    }

    JPH::WheeledVehicleControllerSettings* pControllerSettings = new JPH::WheeledVehicleControllerSettings();

    // Note: big tires trade torque for force, compensate from the 0.3 m Jolt
    // reference wheel so gradeability stays constant across tire sizes
    double dMaxWheelRadiusM = 0.3;

    foreach (const CVehicleWheel& cWheel, vWheels)
    {
        if (cWheel.m_dRadiusM > dMaxWheelRadiusM)
        {
            dMaxWheelRadiusM = cWheel.m_dRadiusM;
        }
    }

    double dEngineTorqueNM = dChassisMassKg * VehicleTorque::ENGINE_TORQUE_NM / VehicleTorque::REFERENCE_MASS_KG * dMaxWheelRadiusM / 0.3;

    pControllerSettings->mEngine.mMaxTorque = float(dEngineTorqueNM);

    // Note: clutch torque is strength times engine to wheel speed gap, a
    // stock strength slips under heavy engines and caps grade traction below
    // demand, inertia sets rev speed for a given torque, both scale with the
    // engine so the coupling stays as rigid as the reference car
    double dTorqueRatio = dEngineTorqueNM / VehicleTorque::REFERENCE_ENGINE_TORQUE_NM;

    pControllerSettings->mEngine.mInertia = float(VehicleTorque::REFERENCE_ENGINE_INERTIA * dTorqueRatio);
    pControllerSettings->mTransmission.mClutchStrength = float(VehicleTorque::REFERENCE_CLUTCH_STRENGTH * dTorqueRatio);

    // Note: vehicles over 3 tons get a truck box with a short first gear for
    // grades, cars keep the Jolt reference ratios, top speed is capped by the
    // controller speed limiter in both cases
    pControllerSettings->mTransmission.mGearRatios.clear();

    if (dChassisMassKg > 3000.0)
    {
        const float fTruckRatios[] = { 5.2f, 3.4f, 2.3f, 1.6f, 1.15f, 0.85f };

        for (unsigned int iGear = 0; iGear < sizeof(fTruckRatios) / sizeof(fTruckRatios[0]); iGear++)
        {
            pControllerSettings->mTransmission.mGearRatios.push_back(fTruckRatios[iGear]);
        }
    }
    else
    {
        const float fCarRatios[] = { 2.66f, 1.78f, 1.3f, 1.0f, 0.74f };

        for (unsigned int iGear = 0; iGear < sizeof(fCarRatios) / sizeof(fCarRatios[0]); iGear++)
        {
            pControllerSettings->mTransmission.mGearRatios.push_back(fCarRatios[iGear]);
        }
    }

    int iPairCount = vDrivenWheels.count() / 2;

    for (int iPair = 0; iPair < iPairCount; iPair++)
    {
        JPH::VehicleDifferentialSettings cDifferential;
        cDifferential.mLeftWheel = vDrivenWheels[iPair * 2];
        cDifferential.mRightWheel = vDrivenWheels[iPair * 2 + 1];
        cDifferential.mEngineTorqueRatio = 1.0f / float(iPairCount);

        pControllerSettings->mDifferentials.push_back(cDifferential);
    }

    cVehicleSettings.mController = pControllerSettings;
    cVehicleSettings.mMaxPitchRollAngle = float(VehicleTorque::MAX_PITCH_ROLL_ANGLE_RAD);

    // Note: wheels sharing a longitudinal station form an axle, the outer
    // pair gets the Jolt sample 1000 N/m bar, stiffer bars couple the axles
    // into a rocking beam on rough ground, roll beyond that is caught by the
    // pitch roll angle limit
    QVector<int> vRemaining;

    for (int iWheel = 0; iWheel < vWheels.count(); iWheel++)
    {
        vRemaining.append(iWheel);
    }

    while (vRemaining.count() > 0)
    {
        int iFirst = vRemaining.takeFirst();

        QVector<int> vAxle;
        vAxle.append(iFirst);

        for (int iScan = vRemaining.count() - 1; iScan >= 0; iScan--)
        {
            if (fabs(vWheels[vRemaining[iScan]].m_vPosition.Z - vWheels[iFirst].m_vPosition.Z) < 0.5)
            {
                vAxle.append(vRemaining.takeAt(iScan));
            }
        }

        if (vAxle.count() < 2)
        {
            continue;
        }

        int iLeft = vAxle[0];
        int iRight = vAxle[0];

        foreach (int iWheel, vAxle)
        {
            if (vWheels[iWheel].m_vPosition.X < vWheels[iLeft].m_vPosition.X)
            {
                iLeft = iWheel;
            }

            if (vWheels[iWheel].m_vPosition.X > vWheels[iRight].m_vPosition.X)
            {
                iRight = iWheel;
            }
        }

        if (iLeft == iRight)
        {
            continue;
        }

        JPH::VehicleAntiRollBar cAntiRollBar;
        cAntiRollBar.mLeftWheel = iLeft;
        cAntiRollBar.mRightWheel = iRight;
        cAntiRollBar.mStiffness = float(VehicleTorque::ANTI_ROLLBAR_REFERENCE_N_PER_M);

        cVehicleSettings.mAntiRollBars.push_back(cAntiRollBar);
    }

    JPH::VehicleConstraint* pConstraint = new JPH::VehicleConstraint(*pChassisBody, cVehicleSettings);
    // Note: the cylinder tester rolls over height field triangle edges, a ray
    // snags on tile steps and ejects the chassis, and it needs no world up
    // vector, the physics frame is a translated ECEF frame
    pConstraint->SetVehicleCollisionTester(new JPH::VehicleCollisionTesterCastCylinder(Layers::MOVING));

    m_pPhysicsSystem->AddConstraint(pConstraint);
    m_pPhysicsSystem->AddStepListener(pConstraint);

    CVehicleRecord cRecord;
    cRecord.m_uChassisBody = pChassisBody->GetID().GetIndexAndSequenceNumber();
    cRecord.m_pConstraint = pConstraint;
    cRecord.m_vCenterOffsetM = vCenterOffsetM;
    m_vVehicles.append(cRecord);

    LOG_DEBUG(QString("Vehicle %1 mass=%2 engine=%3Nm gears=%4 clutch=%5 inertia=%6").arg(m_vVehicles.count() - 1).arg(dChassisMassKg).arg(pControllerSettings->mEngine.mMaxTorque).arg(pControllerSettings->mTransmission.mGearRatios.size()).arg(pControllerSettings->mTransmission.mClutchStrength).arg(pControllerSettings->mEngine.mInertia));

    return m_vVehicles.count() - 1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes a vehicle and its chassis body, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::removeVehicle(int iVehicle)
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    CVehicleRecord cRecord = m_vVehicles[iVehicle];

    // Note: RemoveConstraint releases the manager reference and deletes the
    // constraint, see shutdown()
    m_pPhysicsSystem->RemoveStepListener(cRecord.m_pConstraint);
    m_pPhysicsSystem->RemoveConstraint(cRecord.m_pConstraint);

    JPH::BodyInterface& cBodies = m_pPhysicsSystem->GetBodyInterface();
    cBodies.RemoveBody(JPH::BodyID(cRecord.m_uChassisBody));
    cBodies.DestroyBody(JPH::BodyID(cRecord.m_uChassisBody));

    m_vVehicles.remove(iVehicle);

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
    if (m_bInitialized == false || iSampleCount < 2 || fSpacingXM <= 0.0f || fSpacingZM <= 0.0f)
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

    if (cShapeResult.HasError())
    {
        LOG_METHOD_WARNING("Height field shape creation failed");
        return -1;
    }

    JPH::BodyCreationSettings cBodySettings(
        cShapeResult.Get(),
        CPhysicsMath::toJolt(vPosition),
        CPhysicsMath::toJoltRotation(CPhysicsMath::toQuaternion(mRotation)),
        JPH::EMotionType::Static,
        Layers::NON_MOVING
        );

    // Note: the Jolt default body friction dilutes the authored tire grip
    // through the geometric mean combine, neutral ground leaves the tire
    // curve in charge of gradeability
    cBodySettings.mFriction = 1.0f;

    JPH::BodyInterface& cBodies = m_pPhysicsSystem->GetBodyInterface();
    JPH::Body* pBody = cBodies.CreateBody(cBodySettings);

    if (pBody == nullptr)
    {
        LOG_METHOD_WARNING("Height field body creation failed");
        return -1;
    }

    cBodies.AddBody(pBody->GetID(), JPH::EActivation::DontActivate);

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
    if (m_bInitialized == false || m_mHeightfields.contains(iHeightfield) == false)
    {
        return false;
    }

    JPH::BodyInterface& cBodies = m_pPhysicsSystem->GetBodyInterface();
    cBodies.RemoveBody(JPH::BodyID(m_mHeightfields[iHeightfield].m_uBody));
    cBodies.DestroyBody(JPH::BodyID(m_mHeightfields[iHeightfield].m_uBody));

    m_mHeightfields.remove(iHeightfield);

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
    Removes all vehicles, bodies and height fields
*/
void CPhysicsWorld::clear()
{
    if (m_bInitialized == false)
    {
        return;
    }

    while (m_vVehicles.count() > 0)
    {
        removeVehicle(0);
    }

    clearHeightfields();

    // Note: m_vBodies tracks smoke test bodies only, height fields and
    // vehicles track their own bodies
    JPH::BodyInterface& cBodies = m_pPhysicsSystem->GetBodyInterface();

    foreach (unsigned int uBody, m_vBodies)
    {
        cBodies.RemoveBody(JPH::BodyID(uBody));
        cBodies.DestroyBody(JPH::BodyID(uBody));
    }

    m_vBodies.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Casts a ray against all bodies, fills \a pHitDistanceM on hit. \br\br
    \a vDirection is normalized inside, \a dMaxDistanceM bounds the search.
*/
bool CPhysicsWorld::castRay(const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM)
{
    if (m_bInitialized == false)
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
    if (m_bInitialized == false)
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
    JPH::SpecifiedBroadPhaseLayerFilter cBroadPhaseFilter(BroadPhaseLayers::NON_MOVING);
    JPH::SpecifiedObjectLayerFilter cObjectFilter(Layers::NON_MOVING);

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

/*!
    Sets normalized driver inputs, clamped to [-1, 1] for forward and right, [0, 1] for brakes
*/
bool CPhysicsWorld::setVehicleDriverInput(int iVehicle, double dForwardNorm, double dRightNorm, double dBrakeNorm, double dHandBrakeNorm)
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    JPH::WheeledVehicleController* pController = static_cast<JPH::WheeledVehicleController*>(m_vVehicles[iVehicle].m_pConstraint->GetController());

    if (pController == nullptr)
    {
        return false;
    }

    float fForwardNorm = float(qBound(-1.0, dForwardNorm, 1.0));
    float fRightNorm = float(qBound(-1.0, dRightNorm, 1.0));
    float fBrakeNorm = float(qBound(0.0, dBrakeNorm, 1.0));
    float fHandBrakeNorm = float(qBound(0.0, dHandBrakeNorm, 1.0));

    CVehicleRecord& cRecord = m_vVehicles[iVehicle];

    if (fForwardNorm != cRecord.m_fLastForwardNorm || fRightNorm != cRecord.m_fLastRightNorm || fBrakeNorm != cRecord.m_fLastBrakeNorm || fHandBrakeNorm != cRecord.m_fLastHandBrakeNorm)
    {
        LOG_DEBUG(QString("Vehicle %1 driver fwd=%2 right=%3 brake=%4 hand=%5").arg(iVehicle).arg(fForwardNorm).arg(fRightNorm).arg(fBrakeNorm).arg(fHandBrakeNorm));
        cRecord.m_fLastForwardNorm = fForwardNorm;
        cRecord.m_fLastRightNorm = fRightNorm;
        cRecord.m_fLastBrakeNorm = fBrakeNorm;
        cRecord.m_fLastHandBrakeNorm = fHandBrakeNorm;
    }

    // Note: the engine drives right positive, Jolt steers positive to the
    // left (Wheel mSteerAngle), negate the right input at the boundary so a
    // right command curves right
    pController->SetDriverInput(fForwardNorm, -fRightNorm, fBrakeNorm, fHandBrakeNorm);

    // Note: a parked chassis falls asleep after the Jolt sleep delay and
    // SetDriverInput never wakes it, ResetSleepTimer documents it does not
    // wake a sleeping body either, without activation full throttle revs the
    // engine while the chassis stays frozen
    if (fForwardNorm != 0.0f || fRightNorm != 0.0f || fBrakeNorm != 0.0f)
    {
        m_pPhysicsSystem->GetBodyInterface().ActivateBody(JPH::BodyID(cRecord.m_uChassisBody));
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis position, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehiclePosition(int iVehicle, Math::CVector3& vPosition) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vVehicles[iVehicle].m_uChassisBody));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    vPosition = CPhysicsMath::fromJolt(cLock.GetBody().GetPosition());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis rotation, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleRotation(int iVehicle, Math::CQuaternion& cRotation) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vVehicles[iVehicle].m_uChassisBody));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    cRotation = CPhysicsMath::fromJoltRotation(cLock.GetBody().GetRotation());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis linear velocity, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleLinearVelocity(int iVehicle, Math::CVector3& vVelocity) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vVehicles[iVehicle].m_uChassisBody));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    vVelocity = CPhysicsMath::fromJolt(cLock.GetBody().GetLinearVelocity());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis angular velocity, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleAngularVelocity(int iVehicle, Math::CVector3& vAngularVelocity) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vVehicles[iVehicle].m_uChassisBody));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    vAngularVelocity = CPhysicsMath::fromJolt(cLock.GetBody().GetAngularVelocity());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis center offset, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleCenterOffsetM(int iVehicle, Math::CVector3& vCenterOffsetM) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    vCenterOffsetM = m_vVehicles[iVehicle].m_vCenterOffsetM;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when the given wheel touches a body, false for unknown indexes
*/
bool CPhysicsWorld::vehicleWheelHasContact(int iVehicle, int iWheel) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count() || iWheel < 0)
    {
        return false;
    }

    const JPH::VehicleConstraint* pConstraint = m_vVehicles[iVehicle].m_pConstraint;

    if (iWheel >= int(pConstraint->GetWheels().size()))
    {
        return false;
    }

    return pConstraint->GetWheel(JPH::uint(iWheel))->HasContact();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the number of vehicles in the world
*/
unsigned int CPhysicsWorld::vehicleCount() const
{
    if (m_bInitialized == false)
    {
        return 0;
    }

    return m_vVehicles.count();
}

//-------------------------------------------------------------------------------------------------

/*!
    Copies per step Jolt vehicle state into \a cState, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleDebugState(int iVehicle, CVehicleDebugState& cState) const
{
    if (m_bInitialized == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
    {
        return false;
    }

    const CVehicleRecord& cRecord = m_vVehicles[iVehicle];
    const JPH::VehicleConstraint* pConstraint = cRecord.m_pConstraint;

    if (pConstraint == nullptr)
    {
        return false;
    }

    const JPH::WheeledVehicleController* pController = static_cast<const JPH::WheeledVehicleController*>(pConstraint->GetController());

    if (pController == nullptr)
    {
        return false;
    }

    CVehicleDebugState cFresh;

    cFresh.m_dForwardReadback = double(pController->GetForwardInput());
    cFresh.m_dRightReadback = double(pController->GetRightInput());
    cFresh.m_dBrakeReadback = double(pController->GetBrakeInput());
    cFresh.m_dHandBrakeReadback = double(pController->GetHandBrakeInput());
    cFresh.m_dEngineRPM = double(pController->GetEngine().GetCurrentRPM());
    cFresh.m_iGear = pController->GetTransmission().GetCurrentGear();
    cFresh.m_dClutchFriction = double(pController->GetTransmission().GetClutchFriction());

    const JPH::Wheels& vWheels = pConstraint->GetWheels();

    foreach (const JPH::Wheel* pWheel, vWheels)
    {
        if (pWheel == nullptr)
        {
            continue;
        }

        cFresh.m_vWheelAngularVelocityRS.append(double(pWheel->GetAngularVelocity()));
        cFresh.m_vWheelSuspensionLengthM.append(double(pWheel->GetSuspensionLength()));
        cFresh.m_vWheelContact.append(pWheel->HasContact() ? 1 : 0);
        cFresh.m_vWheelSteerAngleRad.append(double(pWheel->GetSteerAngle()));
        cFresh.m_vWheelSuspensionLambda.append(double(pWheel->GetSuspensionLambda()));
        cFresh.m_vWheelLongitudinalLambda.append(double(pWheel->GetLongitudinalLambda()));

        const JPH::WheelWV* pDriven = static_cast<const JPH::WheelWV*>(pWheel);

        cFresh.m_vWheelLongitudinalSlip.append(double(pDriven->mLongitudinalSlip));
        cFresh.m_vWheelCombinedFriction.append(double(pDriven->mCombinedLongitudinalFriction));
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(cRecord.m_uChassisBody));

    if (cLock.Succeeded() == true)
    {
        const JPH::Body& cBody = cLock.GetBody();

        cFresh.m_bBodyActive = cBody.IsActive();
        cFresh.m_iMotionType = int(cBody.GetMotionType());
        cFresh.m_dBodySpeedMS = double(cBody.GetLinearVelocity().Length());
    }

    cState = cFresh;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Logs per step Jolt vehicle state at debug level, does nothing for an unknown vehicle index
*/
void CPhysicsWorld::logVehicleDebugState(int iVehicle) const
{
    CVehicleDebugState cState;

    if (vehicleDebugState(iVehicle, cState) == false)
    {
        return;
    }

    QString sWheels;

    for (int iWheel = 0; iWheel < cState.m_vWheelAngularVelocityRS.count(); iWheel++)
    {
        double dSteerRad = iWheel < cState.m_vWheelSteerAngleRad.count() ? cState.m_vWheelSteerAngleRad[iWheel] : 0.0;
        double dSlip = iWheel < cState.m_vWheelLongitudinalSlip.count() ? cState.m_vWheelLongitudinalSlip[iWheel] : 0.0;
        double dFriction = iWheel < cState.m_vWheelCombinedFriction.count() ? cState.m_vWheelCombinedFriction[iWheel] : 0.0;
        double dLambda = iWheel < cState.m_vWheelSuspensionLambda.count() ? cState.m_vWheelSuspensionLambda[iWheel] : 0.0;
        double dLongLambda = iWheel < cState.m_vWheelLongitudinalLambda.count() ? cState.m_vWheelLongitudinalLambda[iWheel] : 0.0;

        sWheels += QString("[%1 ang=%2 susp=%3 contact=%4 steer=%5 slip=%6 fric=%7 slam=%8 llam=%9]")
            .arg(iWheel)
            .arg(cState.m_vWheelAngularVelocityRS[iWheel], 0, 'f', 2)
            .arg(cState.m_vWheelSuspensionLengthM[iWheel], 0, 'f', 3)
            .arg(cState.m_vWheelContact[iWheel])
            .arg(dSteerRad, 0, 'f', 3)
            .arg(dSlip, 0, 'f', 3)
            .arg(dFriction, 0, 'f', 3)
            .arg(dLambda, 0, 'f', 1)
            .arg(dLongLambda, 0, 'f', 1);
    }

    LOG_DEBUG(QString("Jolt veh=%1 active=%2 motion=%3 speed=%4 fwd=%5 right=%6 brake=%7 hand=%8 rpm=%9 gear=%10 clutch=%11 wheels=%12")
        .arg(iVehicle)
        .arg(cState.m_bBodyActive ? 1 : 0)
        .arg(cState.m_iMotionType)
        .arg(cState.m_dBodySpeedMS, 0, 'f', 3)
        .arg(cState.m_dForwardReadback, 0, 'f', 2)
        .arg(cState.m_dRightReadback, 0, 'f', 2)
        .arg(cState.m_dBrakeReadback, 0, 'f', 2)
        .arg(cState.m_dHandBrakeReadback, 0, 'f', 2)
        .arg(cState.m_dEngineRPM, 0, 'f', 1)
        .arg(cState.m_iGear)
        .arg(cState.m_dClutchFriction, 0, 'f', 2)
        .arg(sWheels));
}

//-------------------------------------------------------------------------------------------------
