
#pragma once

// Qt
#include <QMap>
#include <QVector>

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CQuaternion.h"

//-------------------------------------------------------------------------------------------------

namespace JPH
{
    class PhysicsSystem;
    class JobSystemThreadPool;
    class TempAllocatorImpl;
    class VehicleConstraint;
}

//-------------------------------------------------------------------------------------------------

/*!
    Wheel settings for CPhysicsWorld::addVehicle. \br\br
    One entry maps to one Jolt wheel in a VehicleConstraint. Positions are in
    meters in the chassis local frame, forward is +Z, up is +Y.
*/
struct CVehicleWheel
{
    Math::CVector3 m_vPosition;                 ///< Suspension attachment point in chassis local frame
    double m_dRadiusM = 0.4;                    ///< Tire radius in meters
    double m_dWidthM = 0.2;                     ///< Tire width in meters
    double m_dSuspensionMinLengthM = 0.35;      ///< Suspension length at full bump in meters
    double m_dSuspensionMaxLengthM = 0.65;      ///< Suspension length at full droop in meters
    double m_dSuspensionFrequencyHz = 1.5;      ///< Suspension spring frequency in hertz
    double m_dSuspensionDampingNorm = 0.5;      ///< Suspension spring damping, 0 to 1
    double m_dLongitudinalFrictionNorm = 1.0;   ///< Tire friction level along rolling direction
    double m_dLateralFrictionNorm = 1.0;        ///< Tire friction level along sideways direction
    double m_dMaxSteerAngleRad = 0.0;           ///< Maximum steer angle in radians, 0 for a fixed wheel
    bool m_bDriven = false;                     ///< Engine torque reaches this wheel
    bool m_bHandBrake = false;                  ///< Hand brake torque reaches this wheel
};

//-------------------------------------------------------------------------------------------------

/*!
    Per step Jolt vehicle state for diagnostics. \br\br
    Read with CPhysicsWorld::vehicleDebugState, all values are copies taken
    under the body lock, safe to log from the stepping thread.
*/
struct CVehicleDebugState
{
    bool m_bBodyActive = false;               ///< False when the chassis sleeps, sleeping bodies skip integration
    int m_iMotionType = -1;                   ///< Jolt EMotionType as int
    double m_dBodySpeedMS = 0.0;              ///< Chassis linear velocity magnitude in meters per second
    double m_dForwardReadback = 0.0;          ///< Forward input read back from the Jolt controller
    double m_dRightReadback = 0.0;            ///< Right input read back from the Jolt controller
    double m_dBrakeReadback = 0.0;            ///< Brake input read back from the Jolt controller
    double m_dHandBrakeReadback = 0.0;        ///< Hand brake input read back from the Jolt controller
    double m_dEngineRPM = 0.0;                ///< Engine speed in revolutions per minute
    int m_iGear = 0;                          ///< Current transmission gear index
    double m_dClutchFriction = 0.0;           ///< Current clutch friction, 0 to 1
    QVector<double> m_vWheelAngularVelocityRS;///< Wheel spin in radians per second, one entry per wheel
    QVector<double> m_vWheelSuspensionLengthM;///< Suspension length in meters, one entry per wheel
    QVector<int> m_vWheelContact;             ///< Wheel ground contact flag, 1 or 0 per wheel
    QVector<double> m_vWheelSteerAngleRad;    ///< Wheel steer angle in radians, Jolt positive is left, one entry per wheel
    QVector<double> m_vWheelLongitudinalSlip; ///< Tire slip ratio, 0 is rolling, 1 is locked sliding, one entry per wheel
    QVector<double> m_vWheelCombinedFriction; ///< Combined tire and ground friction coefficient, one entry per wheel
    QVector<double> m_vWheelSuspensionLambda; ///< Suspension impulse in newton seconds, scales the friction cap, one entry per wheel
    QVector<double> m_vWheelLongitudinalLambda; ///< Applied longitudinal impulse in newton seconds, the delivered push, one entry per wheel
};

//-------------------------------------------------------------------------------------------------

/*!
    Isolated owner of a Jolt physics world. \br\br
    Single dependency direction: engine code uses this class, this class uses
    Jolt, Jolt never calls back into engine code except through the CLogger
    trace and assert handlers set at initialization. \br\br
    Positions are single precision Jolt vectors expressed in a caller provided
    local frame. Geo referenced to local frame conversion stays at the mapping
    layer (vehicles, height fields), never in this class. \br\br
    The job pool owns its threads; it is not bridged to CWorkerManager so a
    physics step can never block a worker thread waiting on another worker.
*/
class QUICK3D_EXPORT CPhysicsWorld
{

public:

    //! Builds an uninitialized world, call initialize() before use
    CPhysicsWorld();

    //! Releases Jolt resources
    virtual ~CPhysicsWorld();

    //! Creates the physics system and its job pool, returns false on failure
    bool initialize();

    //! Destroys the physics system and its job pool
    void shutdown();

    //! Returns true after a successful initialize() without shutdown()
    bool isInitialized() const { return m_bInitialized; }

    //! Advances the simulation by the given step in seconds, fixed step expected
    void step(double dDeltaTimeSeconds);

    //! Sets the gravity vector in meters per second per second, default is (0, -9.81, 0)
    void setGravity(const Math::CVector3& vGravity);

    //! Returns the current gravity vector in meters per second per second
    Math::CVector3 gravity() const;

    //! Adds a static box, returns the body index or -1 on failure
    int addStaticBox(const Math::CVector3& vHalfExtents, const Math::CVector3& vPosition);

    //! Adds a dynamic sphere, returns the body index or -1 on failure
    int addDynamicSphere(double dRadius, const Math::CVector3& vPosition);

    //! Reads a body position, returns false for an unknown body index
    bool bodyPosition(int iBody, Math::CVector3& vPosition) const;

    //! Returns the number of bodies in the world
    unsigned int bodyCount() const;

    //! Adds a dynamic chassis with a VehicleConstraint, returns the vehicle index or -1 on failure. \br\br
    //! \a vPosition is the vehicle origin, the body spawns shifted by \a vCenterOffsetM rotated into the chassis frame.
    int addVehicle(const Math::CVector3& vChassisHalfExtentsM, double dChassisMassKg, const Math::CVector3& vPosition, const QVector<CVehicleWheel>& vWheels, const Math::CQuaternion& cRotation = Math::CQuaternion(0.0, 0.0, 0.0, 1.0), const Math::CVector3& vCenterOffsetM = Math::CVector3(0.0, 0.0, 0.0));

    //! Removes a vehicle and its chassis body, returns false for an unknown vehicle index
    bool removeVehicle(int iVehicle);

    //! Sets normalized driver inputs, clamped to [-1, 1] for forward and right, [0, 1] for brakes
    bool setVehicleDriverInput(int iVehicle, double dForwardNorm, double dRightNorm, double dBrakeNorm, double dHandBrakeNorm);

    //! Reads a vehicle chassis position, returns false for an unknown vehicle index
    bool vehiclePosition(int iVehicle, Math::CVector3& vPosition) const;

    //! Reads a vehicle chassis rotation, returns false for an unknown vehicle index
    bool vehicleRotation(int iVehicle, Math::CQuaternion& cRotation) const;

    //! Reads a vehicle chassis linear velocity, returns false for an unknown vehicle index
    bool vehicleLinearVelocity(int iVehicle, Math::CVector3& vVelocity) const;

    //! Reads a vehicle chassis angular velocity, returns false for an unknown vehicle index
    bool vehicleAngularVelocity(int iVehicle, Math::CVector3& vAngularVelocity) const;

    //! Reads a vehicle chassis center offset, returns false for an unknown vehicle index
    bool vehicleCenterOffsetM(int iVehicle, Math::CVector3& vCenterOffsetM) const;

    //! Returns true when the given wheel touches a body, false for unknown indexes
    bool vehicleWheelHasContact(int iVehicle, int iWheel) const;

    //! Copies per step Jolt vehicle state into \a cState, returns false for an unknown vehicle index
    bool vehicleDebugState(int iVehicle, CVehicleDebugState& cState) const;

    //! Logs per step Jolt vehicle state at debug level, does nothing for an unknown vehicle index
    void logVehicleDebugState(int iVehicle) const;

    //! Adds a static height field grid, returns the height field id or -1 on failure. \br\br
    //! Ids grow monotonically and stay valid until removal. \br\br
    //! \a vHeights holds \a iSampleCount rows of \a iSampleCount samples in meters, row 0 is the north edge, column 0 the west edge. \br
    //! \a vPosition is the grid center at zero datum, the local frame has +X east, +Y up and +Z north.
    int addHeightfield(const QVector<float>& vHeights, int iSampleCount, float fSpacingXM, float fSpacingZM, const Math::CVector3& vPosition, const Math::CMatrix4& mRotation);

    //! Removes a height field, returns false for an unknown height field id
    bool removeHeightfield(int iHeightfield);

    //! Removes all height fields
    void clearHeightfields();

    //! Removes all vehicles, bodies and height fields
    void clear();

    //! Casts a ray against all bodies, fills \a pHitDistanceM on hit. \br\br
    //! \a vDirection is normalized inside, \a dMaxDistanceM bounds the search.
    bool castRay(const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM);

    //! Casts a ray against registered height fields only, fills \a pHitDistanceM on hit. \br\br
    //! Vehicles and other static bodies are ignored, use it for terrain ground queries.
    bool castTerrainRay(const Math::CVector3& vOrigin, const Math::CVector3& vDirection, double dMaxDistanceM, double* pHitDistanceM);

    //! Returns the number of height fields in the world
    unsigned int heightfieldCount() const;

    //! Returns the number of vehicles in the world
    unsigned int vehicleCount() const;

private:

    Q_DISABLE_COPY(CPhysicsWorld)

    //! Links a chassis body to its owned vehicle constraint
    struct CVehicleRecord
    {
        unsigned int m_uChassisBody = 0;
        JPH::VehicleConstraint* m_pConstraint = nullptr;
        Math::CVector3 m_vCenterOffsetM;        ///< Vehicle origin to body center in chassis frame
        float m_fLastForwardNorm = 2.0f;        ///< Last driver forward input, 2.0 forces the first change log
        float m_fLastRightNorm = 2.0f;          ///< Last driver right input
        float m_fLastBrakeNorm = 2.0f;          ///< Last driver brake input
        float m_fLastHandBrakeNorm = 2.0f;      ///< Last driver hand brake input
    };

    //! Links a height field index to its static body
    struct CHeightfieldRecord
    {
        unsigned int m_uBody = 0;
    };

    bool m_bInitialized;
    JPH::PhysicsSystem* m_pPhysicsSystem;
    JPH::JobSystemThreadPool* m_pJobSystem;
    JPH::TempAllocatorImpl* m_pTempAllocator;
    QVector<unsigned int> m_vBodies;
    QVector<CVehicleRecord> m_vVehicles;
    QMap<int, CHeightfieldRecord> m_mHeightfields;
    int m_iNextHeightfieldId;
};

//-------------------------------------------------------------------------------------------------
