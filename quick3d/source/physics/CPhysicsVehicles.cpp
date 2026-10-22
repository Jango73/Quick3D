
// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/BodyInterface.h>

// Application
#include "CLogger.h"
#include "CPhysicsWorld.h"
#include "internal/CPhysicsWorldInternal.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Applies the Jolt tire grip shape scaled by the authored level.
*/
/***************************************************************************/
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
    Runs \a cReader under a read lock on the chassis body, false for an unknown vehicle
*/
bool CPhysicsWorld::withLockedChassisBody(int iVehicle, const std::function<void(const JPH::Body&)>& cReader) const
{
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count() || cReader == nullptr)
    {
        return false;
    }

    JPH::BodyLockRead cLock(m_pPhysicsSystem->GetBodyLockInterface(), JPH::BodyID(m_vVehicles[iVehicle].m_uChassisBody));

    if (cLock.Succeeded() == false)
    {
        return false;
    }

    cReader(cLock.GetBody());

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a dynamic chassis with a VehicleConstraint, returns the vehicle index or -1 on failure. \br\br
    \a vPosition is the vehicle origin, the body spawns shifted by \a vCenterOffsetM rotated into the chassis frame.
*/
int CPhysicsWorld::addVehicle(const Math::CVector3& vChassisHalfExtentsM, double dChassisMassKg, const Math::CVector3& vPosition, const QVector<CVehicleWheel>& vWheels, const Math::CQuaternion& cRotation, const Math::CVector3& vCenterOffsetM)
{
    if (isReady() == false)
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
    JPH::Vec3 vHalfExtents = CPhysicsJolt::toJolt(vChassisHalfExtentsM);
    JPH::Shape::ShapeResult cChassisResult = JPH::OffsetCenterOfMassShapeSettings(JPH::Vec3(0.0f, -vHalfExtents.GetY(), 0.0f), new JPH::BoxShape(vHalfExtents)).Create();
    JPH::RefConst<JPH::Shape> pChassisShape = cChassisResult.Get();

    if (isValidShape(cChassisResult, pChassisShape) == false)
    {
        LOG_METHOD_ERROR("Chassis shape creation failed");
        return -1;
    }

    JPH::Quat cChassisRotation = CPhysicsJolt::toJoltRotation(cRotation).Normalized();
    JPH::Vec3 vBodyPosition = CPhysicsJolt::toJolt(vPosition) + cChassisRotation * CPhysicsJolt::toJolt(vCenterOffsetM);

    JPH::BodyCreationSettings cBodySettings(pChassisShape, vBodyPosition, cChassisRotation, JPH::EMotionType::Dynamic, CPhysicsLayers::MOVING);
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
        // constraint expects them in the body center frame. Jolt hangs the
        // wheel below a chassis hardpoint along -Y, so the attachment sits
        // one mid travel above the declared wheel center: at mid travel the
        // simulated wheel center matches the authored position.
        double dSuspMidM = 0.5 * (cWheel.m_dSuspensionMinLengthM + cWheel.m_dSuspensionMaxLengthM);
        pWheelSettings->mPosition = CPhysicsJolt::toJolt(cWheel.m_vPosition - vCenterOffsetM + Math::CVector3(0.0, dSuspMidM, 0.0));
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
        pWheelSettings->mMaxBrakeTorque = float(dChassisMassKg * CVehicleTuning::BRAKE_TORQUE_NM / CVehicleTuning::REFERENCE_MASS_KG);

        if (cWheel.m_bHandBrake == false)
        {
            pWheelSettings->mMaxHandBrakeTorque = 0.0f;
        }
        else
        {
            pWheelSettings->mMaxHandBrakeTorque = float(dChassisMassKg * CVehicleTuning::HANDBRAKE_TORQUE_NM / CVehicleTuning::REFERENCE_MASS_KG);
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

    double dEngineTorqueNM = dChassisMassKg * CVehicleTuning::ENGINE_TORQUE_NM / CVehicleTuning::REFERENCE_MASS_KG * dMaxWheelRadiusM / 0.3;

    pControllerSettings->mEngine.mMaxTorque = float(dEngineTorqueNM);

    // Note: clutch torque is strength times engine to wheel speed gap, a
    // stock strength slips under heavy engines and caps grade traction below
    // demand, inertia sets rev speed for a given torque, both scale with the
    // engine so the coupling stays as rigid as the reference car
    double dTorqueRatio = dEngineTorqueNM / CVehicleTuning::REFERENCE_ENGINE_TORQUE_NM;

    pControllerSettings->mEngine.mInertia = float(CVehicleTuning::REFERENCE_ENGINE_INERTIA * dTorqueRatio);
    pControllerSettings->mTransmission.mClutchStrength = float(CVehicleTuning::REFERENCE_CLUTCH_STRENGTH * dTorqueRatio);

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
    cVehicleSettings.mMaxPitchRollAngle = float(CVehicleTuning::MAX_PITCH_ROLL_ANGLE_RAD);

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
        cAntiRollBar.mStiffness = float(CVehicleTuning::ANTI_ROLLBAR_REFERENCE_N_PER_M);

        cVehicleSettings.mAntiRollBars.push_back(cAntiRollBar);
    }

    JPH::VehicleConstraint* pConstraint = new JPH::VehicleConstraint(*pChassisBody, cVehicleSettings);
    // Note: the cylinder tester rolls over height field triangle edges, a ray
    // snags on tile steps and ejects the chassis, and it needs no world up
    // vector, the physics frame is a translated ECEF frame
    pConstraint->SetVehicleCollisionTester(new JPH::VehicleCollisionTesterCastCylinder(CPhysicsLayers::MOVING));

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
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
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
    Wakes every vehicle chassis, ground changes never leave sleepers floating on vanished support
*/
void CPhysicsWorld::activateVehicles()
{
    if (isReady() == false)
    {
        return;
    }

    foreach (const CVehicleRecord& cRecord, m_vVehicles)
    {
        m_pPhysicsSystem->GetBodyInterface().ActivateBody(JPH::BodyID(cRecord.m_uChassisBody));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets normalized driver inputs, clamped to [-1, 1] for forward and right, [0, 1] for brakes
*/
bool CPhysicsWorld::setVehicleDriverInput(int iVehicle, double dForwardNorm, double dRightNorm, double dBrakeNorm, double dHandBrakeNorm)
{
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
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

    QVector<float> vDriverInput;
    vDriverInput.append(fForwardNorm);
    vDriverInput.append(fRightNorm);
    vDriverInput.append(fBrakeNorm);
    vDriverInput.append(fHandBrakeNorm);

    if (cRecord.m_cDriverInputLog.hasChanged(vDriverInput) == true)
    {
        LOG_DEBUG(QString("Vehicle %1 driver fwd=%2 right=%3 brake=%4 hand=%5").arg(iVehicle).arg(fForwardNorm).arg(fRightNorm).arg(fBrakeNorm).arg(fHandBrakeNorm));
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
    return withLockedChassisBody(iVehicle, [&](const JPH::Body& cBody)
    {
        vPosition = CPhysicsJolt::fromJolt(cBody.GetPosition());
    });
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis rotation, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleRotation(int iVehicle, Math::CQuaternion& cRotation) const
{
    return withLockedChassisBody(iVehicle, [&](const JPH::Body& cBody)
    {
        cRotation = CPhysicsJolt::fromJoltRotation(cBody.GetRotation());
    });
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis linear velocity, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleLinearVelocity(int iVehicle, Math::CVector3& vVelocity) const
{
    return withLockedChassisBody(iVehicle, [&](const JPH::Body& cBody)
    {
        vVelocity = CPhysicsJolt::fromJolt(cBody.GetLinearVelocity());
    });
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis angular velocity, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleAngularVelocity(int iVehicle, Math::CVector3& vAngularVelocity) const
{
    return withLockedChassisBody(iVehicle, [&](const JPH::Body& cBody)
    {
        vAngularVelocity = CPhysicsJolt::fromJolt(cBody.GetAngularVelocity());
    });
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads a vehicle chassis center offset, returns false for an unknown vehicle index
*/
bool CPhysicsWorld::vehicleCenterOffsetM(int iVehicle, Math::CVector3& vCenterOffsetM) const
{
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
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
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count() || iWheel < 0)
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
    if (isReady() == false)
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
    if (isReady() == false || iVehicle < 0 || iVehicle >= m_vVehicles.count())
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

    withLockedChassisBody(iVehicle, [&](const JPH::Body& cBody)
    {
        cFresh.m_bBodyActive = cBody.IsActive();
        cFresh.m_iMotionType = int(cBody.GetMotionType());
        cFresh.m_dBodySpeedMS = double(cBody.GetLinearVelocity().Length());
    });

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
