
#pragma once

// Qt
#include <QMutex>

// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// Application
#include "CPhysicsJolt.h"

//-------------------------------------------------------------------------------------------------

/*!
    Object layers of the physics world, class scope on purpose. \br\br
    Kept inside a struct so no namespace scope state leaks: static bodies
    never collide with each other, moving bodies collide with everything.
*/
struct CPhysicsLayers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer COUNT = 2;
};

//-------------------------------------------------------------------------------------------------

/*!
    Broad phase layers mirroring CPhysicsLayers, class scope on purpose
*/
struct CPhysicsBroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING = JPH::BroadPhaseLayer(0);
    static constexpr JPH::BroadPhaseLayer MOVING = JPH::BroadPhaseLayer(1);
    static constexpr unsigned int COUNT = 2;
};

//-------------------------------------------------------------------------------------------------

/*!
    Vehicle tuning constants, class scope on purpose. \br\br
    Jolt vehicle defaults target a 1500 kg reference car with 0.3 m wheels,
    torque scales with chassis mass and compensates the largest tire radius
    so wheel force per ton stays constant, grade torque comes from the truck
    first gear, top speed from the controller speed limiter, never from
    oversized torque.
*/
struct CVehicleTuning
{
    static constexpr double REFERENCE_MASS_KG = 1500.0;
    static constexpr double ENGINE_TORQUE_NM = 250.0;
    static constexpr double BRAKE_TORQUE_NM = 1500.0;
    static constexpr double HANDBRAKE_TORQUE_NM = 4000.0;
    static constexpr double REFERENCE_ENGINE_TORQUE_NM = 500.0;
    static constexpr double REFERENCE_CLUTCH_STRENGTH = 10.0;
    static constexpr double REFERENCE_ENGINE_INERTIA = 0.5;
    static constexpr double ANTI_ROLLBAR_REFERENCE_N_PER_M = 1000.0;
    static constexpr double MAX_PITCH_ROLL_ANGLE_RAD = 1.0471975512;
};

//-------------------------------------------------------------------------------------------------

/*!
    Maps object layers to broad phase layers, owned by CPhysicsWorld
*/
class CBroadPhaseLayerMapper final : public JPH::BroadPhaseLayerInterface
{

public:

    unsigned int GetNumBroadPhaseLayers() const Q_DECL_OVERRIDE
    {
        return CPhysicsBroadPhaseLayers::COUNT;
    }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer iLayer) const Q_DECL_OVERRIDE
    {
        if (iLayer == CPhysicsLayers::NON_MOVING)
        {
            return CPhysicsBroadPhaseLayers::NON_MOVING;
        }

        return CPhysicsBroadPhaseLayers::MOVING;
    }

#if defined(JPH_PROFILE_ENABLED) || defined(JPH_EXTERNAL_PROFILE)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer iLayer) const Q_DECL_OVERRIDE
    {
        if (iLayer == CPhysicsBroadPhaseLayers::NON_MOVING)
        {
            return "NON_MOVING";
        }

        return "MOVING";
    }
#endif
};

//-------------------------------------------------------------------------------------------------

/*!
    Broad phase filter: static bodies never collide at broad phase level
*/
class CObjectBroadPhaseFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
{

public:

    bool ShouldCollide(JPH::ObjectLayer iObject, JPH::BroadPhaseLayer iBroadPhase) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(iBroadPhase)

        if (iObject == CPhysicsLayers::NON_MOVING)
        {
            return false;
        }

        return true;
    }
};

//-------------------------------------------------------------------------------------------------

/*!
    Layer pair filter: a static body only collides with a moving body
*/
class CObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter
{

public:

    bool ShouldCollide(JPH::ObjectLayer iFirst, JPH::ObjectLayer iSecond) const Q_DECL_OVERRIDE
    {
        if (iFirst == CPhysicsLayers::NON_MOVING)
        {
            return iSecond == CPhysicsLayers::MOVING;
        }

        return true;
    }
};

//-------------------------------------------------------------------------------------------------

/*!
    Process wide Jolt library owner. \br\br
    Jolt requires a single RegisterTypes() and Factory instance per process
    while several CPhysicsWorld instances come and go (scenes, tests). The
    reference count is the only process scope state of the physics layer,
    guarded by its own mutex and documented here instead of scattered
    function local statics.
*/
class CPhysicsLibrary
{

public:

    //! Registers Jolt types on first use, counts one more world user
    static void acquire();

    //! Releases one world user, unregisters Jolt types on last release
    static void release();

private:

    static QMutex s_Mutex;
    static int s_iUsers;
};

//-------------------------------------------------------------------------------------------------

/*!
    Creates a body from \a pShape and adds it to \a pSystem in one step. \br\br
    Returns an invalid id on failure, \a fFriction below zero keeps the Jolt
    default, callers set 1.0 for neutral height field ground.
*/
JPH::BodyID createAndAddBody(JPH::PhysicsSystem* pSystem, const JPH::ShapeRefC& pShape, const JPH::Vec3& vPosition, const JPH::Quat& cRotation, JPH::EMotionType eMotion, JPH::ObjectLayer uLayer, JPH::EActivation eActivation, float fFriction = -1.0f);

/*!
    Creates a body from \a pShape without adding it, for constraints. \br\br
    Returns nullptr on failure, \a fFriction below zero keeps the Jolt
    default. The caller adds the returned body to the system.
*/
JPH::Body* createBody(JPH::PhysicsSystem* pSystem, const JPH::ShapeRefC& pShape, const JPH::Vec3& vPosition, const JPH::Quat& cRotation, JPH::EMotionType eMotion, JPH::ObjectLayer uLayer, float fFriction = -1.0f);

/*!
    Returns true when \a cResult holds a usable shape in \a pShape
*/
bool isValidShape(const JPH::Shape::ShapeResult& cResult, const JPH::ShapeRefC& pShape);

//-------------------------------------------------------------------------------------------------
