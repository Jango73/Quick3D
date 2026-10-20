
// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyInterface.h>

// Qt
#include <QMutexLocker>
#include <QThread>

// Application
#include "CLogger.h"
#include "CPhysicsWorld.h"
#include "internal/CPhysicsWorldInternal.h"

//-------------------------------------------------------------------------------------------------

QMutex CPhysicsLibrary::s_Mutex;
int CPhysicsLibrary::s_iUsers = 0;

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

/*!
    Registers Jolt types on first use, counts one more world user
*/
void CPhysicsLibrary::acquire()
{
    QMutexLocker cLocker(&s_Mutex);

    if (s_iUsers == 0)
    {
        JPH::RegisterDefaultAllocator();
        JPH::Trace = joltTrace;
#ifdef JPH_ENABLE_ASSERTS
        JPH::AssertFailed = joltAssertFailed;
#endif
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
    }

    s_iUsers++;
}

//-------------------------------------------------------------------------------------------------

/*!
    Releases one world user, unregisters Jolt types on last release
*/
void CPhysicsLibrary::release()
{
    QMutexLocker cLocker(&s_Mutex);

    if (s_iUsers > 0)
    {
        s_iUsers--;
    }

    if (s_iUsers == 0)
    {
        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates a body from \a pShape and adds it to \a pSystem in one step
*/
JPH::BodyID createAndAddBody(JPH::PhysicsSystem* pSystem, const JPH::ShapeRefC& pShape, const JPH::Vec3& vPosition, const JPH::Quat& cRotation, JPH::EMotionType eMotion, JPH::ObjectLayer uLayer, JPH::EActivation eActivation, float fFriction)
{
    if (pSystem == nullptr || pShape == nullptr)
    {
        return JPH::BodyID();
    }

    JPH::BodyCreationSettings cSettings(pShape, vPosition, cRotation, eMotion, uLayer);

    if (fFriction >= 0.0f)
    {
        cSettings.mFriction = fFriction;
    }

    return pSystem->GetBodyInterface().CreateAndAddBody(cSettings, eActivation);
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates a body from \a pShape without adding it, for constraints
*/
JPH::Body* createBody(JPH::PhysicsSystem* pSystem, const JPH::ShapeRefC& pShape, const JPH::Vec3& vPosition, const JPH::Quat& cRotation, JPH::EMotionType eMotion, JPH::ObjectLayer uLayer, float fFriction)
{
    if (pSystem == nullptr || pShape == nullptr)
    {
        return nullptr;
    }

    JPH::BodyCreationSettings cSettings(pShape, vPosition, cRotation, eMotion, uLayer);

    if (fFriction >= 0.0f)
    {
        cSettings.mFriction = fFriction;
    }

    return pSystem->GetBodyInterface().CreateBody(cSettings);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when \a cResult holds a usable shape in \a pShape
*/
bool isValidShape(const JPH::Shape::ShapeResult& cResult, const JPH::ShapeRefC& pShape)
{
    return cResult.IsValid() && pShape != nullptr;
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
    , m_pBroadPhaseMapper(nullptr)
    , m_pBroadPhaseFilter(nullptr)
    , m_pLayerFilter(nullptr)
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
    QMutexLocker cLocker(&m_mInitMutex);

    if (m_bInitialized == true)
    {
        return true;
    }

    CPhysicsLibrary::acquire();

    int iThreads = QThread::idealThreadCount() - 1;

    if (iThreads < 1)
    {
        iThreads = 1;
    }

    if (iThreads > 8)
    {
        iThreads = 8;
    }

    m_pBroadPhaseMapper = new CBroadPhaseLayerMapper();
    m_pBroadPhaseFilter = new CObjectBroadPhaseFilter();
    m_pLayerFilter = new CObjectLayerPairFilter();

    m_pJobSystem = new JPH::JobSystemThreadPool(256, 8, iThreads);
    m_pTempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    m_pPhysicsSystem = new JPH::PhysicsSystem();

    m_pPhysicsSystem->Init(1024, 0, 1024, 1024, *m_pBroadPhaseMapper, *m_pBroadPhaseFilter, *m_pLayerFilter);
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
    QMutexLocker cLocker(&m_mInitMutex);

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
    delete m_pBroadPhaseMapper;
    delete m_pBroadPhaseFilter;
    delete m_pLayerFilter;

    m_pPhysicsSystem = nullptr;
    m_pJobSystem = nullptr;
    m_pTempAllocator = nullptr;
    m_pBroadPhaseMapper = nullptr;
    m_pBroadPhaseFilter = nullptr;
    m_pLayerFilter = nullptr;
    m_vBodies.clear();
    m_bInitialized = false;

    CPhysicsLibrary::release();
}

//-------------------------------------------------------------------------------------------------

/*!
    Advances the simulation by the given step in seconds, fixed step expected
*/
void CPhysicsWorld::step(double dDeltaTimeSeconds)
{
    if (isReady() == false || dDeltaTimeSeconds <= 0.0)
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
    if (isReady() == false)
    {
        return;
    }

    m_pPhysicsSystem->SetGravity(CPhysicsJolt::toJolt(vGravity));
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the current gravity vector in meters per second per second
*/
Math::CVector3 CPhysicsWorld::gravity() const
{
    if (isReady() == false)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    return CPhysicsJolt::fromJolt(m_pPhysicsSystem->GetGravity());
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a static box, returns the body index or -1 on failure
*/
int CPhysicsWorld::addStaticBox(const Math::CVector3& vHalfExtents, const Math::CVector3& vPosition)
{
    if (isReady() == false)
    {
        LOG_METHOD_ERROR("World not initialized");
        return -1;
    }

    JPH::BoxShapeSettings cShapeSettings(CPhysicsJolt::toJolt(vHalfExtents));
    JPH::Shape::ShapeResult cShapeResult = cShapeSettings.Create();
    JPH::ShapeRefC pShape = cShapeResult.Get();

    if (isValidShape(cShapeResult, pShape) == false)
    {
        LOG_METHOD_ERROR("Box shape creation failed");
        return -1;
    }

    JPH::BodyID iBodyID = createAndAddBody(m_pPhysicsSystem, pShape, CPhysicsJolt::toJolt(vPosition), JPH::Quat::sIdentity(), JPH::EMotionType::Static, CPhysicsLayers::NON_MOVING, JPH::EActivation::DontActivate);

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
    if (isReady() == false)
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

    if (isValidShape(cShapeResult, pShape) == false)
    {
        LOG_METHOD_ERROR("Sphere shape creation failed");
        return -1;
    }

    JPH::BodyID iBodyID = createAndAddBody(m_pPhysicsSystem, pShape, CPhysicsJolt::toJolt(vPosition), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, CPhysicsLayers::MOVING, JPH::EActivation::Activate);

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
    if (isReady() == false || iBody < 0 || iBody >= m_vBodies.count())
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vBodies[iBody]));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    vPosition = CPhysicsJolt::fromJolt(cLock.GetBody().GetPosition());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the number of bodies in the world
*/
unsigned int CPhysicsWorld::bodyCount() const
{
    if (isReady() == false)
    {
        return 0;
    }

    return m_pPhysicsSystem->GetNumBodies();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all vehicles, bodies and height fields
*/
void CPhysicsWorld::clear()
{
    if (isReady() == false)
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
