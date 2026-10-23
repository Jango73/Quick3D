
// Qt
#include <QKeyEvent>

// STL
#include <cmath>

// Application
#include "CTestVehicle.h"
#include "CQuaternion.h"
#include "CVehicle.h"
#include "CWheel.h"
#include "CForceController.h"
#include "CPhysicsWorld.h"
#include "CXMLNode.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Builds a vehicle with four driven wheels, front wheels steered.
*/
/***************************************************************************/
static QSP<CVehicle> createFourWheelTestVehicle()
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    pVehicle->setMass_kg(1200.0);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QSP<CWheel> pWheel(new CWheel(nullptr));

        pWheel->setDriven(true);

        // Note: the loader names the parent, which switches position() and
        // setPosition() to the local frame. setParent() is avoided, it would
        // wrap this in a second ownership chain. The name must be set before
        // the position, setPosition on a root object writes the geolocation.
        pWheel->setParentName(QString("TestVehicle"));
        pWheel->setPosition(Math::CVector3((iWheel % 2 == 0) ? 0.9 : -0.9, -0.27, (iWheel < 2) ? 1.2 : -1.2));

        if (iWheel < 2)
        {
            pWheel->setSteered(true);
        }

        pVehicle->childComponents().append(pWheel);
    }

    return pVehicle;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Reads the settled chassis pose and returns its horizontal forward axis.
*/
static bool settledForwardAxis(CPhysicsWorld& oWorld, int iVehicle, Math::CVector3& vPosition, Math::CVector3& vForwardAxis)
{
    Math::CQuaternion cRotation;

    if (oWorld.vehiclePosition(iVehicle, vPosition) == false)
    {
        return false;
    }

    if (oWorld.vehicleRotation(iVehicle, cRotation) == false)
    {
        return false;
    }

    // Note: the drop bounces the chassis, its yaw after settling depends on
    // the machine (Jolt solves on a thread pool sized by core count), the
    // drive assertions follow the settled forward axis instead of world +Z
    vForwardAxis = cRotation * Math::CVector3(0.0, 0.0, 1.0);
    vForwardAxis.Y = 0.0;
    vForwardAxis = vForwardAxis.normalized();

    return vForwardAxis.magnitude() > 0.0;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestVehicle::CTestVehicle()
    : CTestFixture()
{
}

/***************************************************************************/
/*!
* \brief Wheel defaults match the physics wheel defaults.
*/
/***************************************************************************/
void CTestVehicle::testWheelDefaultsMatchPhysicsDefaults()
{
    CWheel oWheel(nullptr);
    CVehicleWheel cPhysicsWheel;

    QVERIFY(oWheel.radiusM() == cPhysicsWheel.m_dRadiusM);
    QVERIFY(oWheel.widthM() == cPhysicsWheel.m_dWidthM);
    QVERIFY(oWheel.driven() == cPhysicsWheel.m_bDriven);
    QVERIFY(oWheel.steered() == (cPhysicsWheel.m_dMaxSteerAngleRad != 0.0));
    QVERIFY(oWheel.suspensionMinLengthM() == cPhysicsWheel.m_dSuspensionMinLengthM);
    QVERIFY(oWheel.suspensionMaxLengthM() == cPhysicsWheel.m_dSuspensionMaxLengthM);
    QVERIFY(oWheel.suspensionFrequencyHz() == cPhysicsWheel.m_dSuspensionFrequencyHz);
    QVERIFY(oWheel.suspensionDampingNorm() == cPhysicsWheel.m_dSuspensionDampingNorm);
    QVERIFY(oWheel.longitudinalFrictionNorm() == cPhysicsWheel.m_dLongitudinalFrictionNorm);
    QVERIFY(oWheel.lateralFrictionNorm() == cPhysicsWheel.m_dLateralFrictionNorm);
    QVERIFY(oWheel.handBrake() == cPhysicsWheel.m_bHandBrake);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Wheel parameters load from the General XML node.
*/
/***************************************************************************/
void CTestVehicle::testWheelLoadsParametersFromXML()
{
    QString sError;
    CXMLNode xComponent = CXMLNode::parseXML(QString("<Component Name=\"Wheel\" Class=\"CWheel\"><Position x=\"0.8\" y=\"0.4\" z=\"3.0\" /><General Radius=\"0.8\" Width=\"0.5\" Driven=\"1\" Steered=\"1\" MaxSteerAngleDeg=\"25.0\" SuspensionMinLength=\"0.2\" SuspensionMaxLength=\"0.5\" SuspensionFrequencyHz=\"2.0\" SuspensionDamping=\"0.5\" LongitudinalFriction=\"1.2\" LateralFriction=\"0.9\" /></Component>"), &sError);

    QVERIFY2(sError.isEmpty() == true, qPrintable(sError));

    CWheel oWheel(nullptr);
    oWheel.setParentName(QString("WheelParent"));
    oWheel.loadParameters(QString(""), xComponent);

    QVERIFY(oWheel.position().X == 0.8);
    QVERIFY(oWheel.position().Y == 0.4);
    QVERIFY(oWheel.position().Z == 3.0);
    QVERIFY(oWheel.radiusM() == 0.8);
    QVERIFY(oWheel.widthM() == 0.5);
    QVERIFY(oWheel.driven() == true);
    QVERIFY(oWheel.steered() == true);
    QVERIFY(oWheel.maxSteerAngleDeg() == 25.0);
    QVERIFY(oWheel.suspensionMinLengthM() == 0.2);
    QVERIFY(oWheel.suspensionMaxLengthM() == 0.5);
    QVERIFY(oWheel.suspensionFrequencyHz() == 2.0);
    QVERIFY(oWheel.suspensionDampingNorm() == 0.5);
    QVERIFY(oWheel.longitudinalFrictionNorm() == 1.2);
    QVERIFY(oWheel.lateralFrictionNorm() == 0.9);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A vehicle builds one physics wheel per wheel child.
*/
/***************************************************************************/
void CTestVehicle::testVehicleBuildsPhysicsWheelsFromChildren()
{
    QSP<CVehicle> pVehicle = createFourWheelTestVehicle();

    QVERIFY(pVehicle->hasWheels() == true);

    QVector<CVehicleWheel> vPhysicsWheels = pVehicle->buildPhysicsWheels();

    QVERIFY(vPhysicsWheels.count() == 4);

    QVERIFY(vPhysicsWheels[0].m_vPosition.X == 0.9);
    QVERIFY(vPhysicsWheels[0].m_vPosition.Z == 1.2);
    QVERIFY(vPhysicsWheels[0].m_bDriven == true);
    QVERIFY(vPhysicsWheels[0].m_dMaxSteerAngleRad > 0.0);

    QVERIFY(vPhysicsWheels[2].m_vPosition.Z == -1.2);
    QVERIFY(vPhysicsWheels[2].m_bDriven == true);
    QVERIFY(vPhysicsWheels[2].m_dMaxSteerAngleRad == 0.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A vehicle without wheels refuses physics attachment.
*/
/***************************************************************************/
void CTestVehicle::testVehicleWithoutWheelsRefusesAttachment()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    CVehicle oVehicle(nullptr);

    QVERIFY(oVehicle.hasWheels() == false);
    QVERIFY(oVehicle.attachPhysicsVehicle(&oWorld, Math::CVector3(0.9, 0.3, 2.0), Math::CVector3(0.0, 2.0, 0.0)) == false);
    QVERIFY(oVehicle.hasPhysicsVehicle() == false);
    QVERIFY(oWorld.vehicleCount() == 0);

    oWorld.shutdown();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Key presses report normalized drive inputs.
*/
/***************************************************************************/
void CTestVehicle::testControllerReportsNormalizedDriveInput()
{
    CForceController oController(nullptr);

    QVERIFY(oController.driveForwardNorm() == 0.0);
    QVERIFY(oController.driveRightNorm() == 0.0);

    QKeyEvent cForwardPress(QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier);
    oController.keyPressEvent(&cForwardPress);

    QVERIFY(oController.driveForwardNorm() == 1.0);

    QKeyEvent cRightPress(QEvent::KeyPress, Qt::Key_C, Qt::NoModifier);
    oController.keyPressEvent(&cRightPress);

    QVERIFY(oController.driveRightNorm() == 1.0);

    QKeyEvent cForwardRelease(QEvent::KeyRelease, Qt::Key_Z, Qt::NoModifier);
    oController.keyReleaseEvent(&cForwardRelease);

    QVERIFY(oController.driveForwardNorm() == 0.0);

    QKeyEvent cBackwardPress(QEvent::KeyPress, Qt::Key_S, Qt::NoModifier);
    oController.keyPressEvent(&cBackwardPress);

    QVERIFY(oController.driveForwardNorm() == -1.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A force controller drives an attached vehicle through driver inputs.
*/
/***************************************************************************/
void CTestVehicle::testWheeledControllerDrivesAttachedVehicle()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);
    QVERIFY(oWorld.addStaticBox(Math::CVector3(50.0, 1.0, 50.0), Math::CVector3(0.0, -1.0, 0.0)) >= 0);

    QSP<CVehicle> pVehicle = createFourWheelTestVehicle();

    // Note: body starts at suspension equilibrium (radius 0.4 minus wheel
    // center -0.27), a 1 cm drop only, a high spawn free falls and tips
    const double dEquilibriumYM = 0.67;
    const double dSpawnMarginM = 0.01;

    QVERIFY(pVehicle->attachPhysicsVehicle(&oWorld, Math::CVector3(0.9, 0.3, 2.0), Math::CVector3(0.0, dEquilibriumYM + dSpawnMarginM, 0.0)) == true);
    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    // Note: the hand brake engages by default, the drive run releases it so
    // the run matches the direct twin (hand input zero), brake behavior is
    // covered by testHandBrakeEngagedByDefault and the heavy test below
    pVehicle->setHandBrakeEngaged(false);

    CForceController oController(nullptr);
    oController.setPositionTarget(pVehicle);

    // Note: settle at equilibrium before driving, a falling chassis yaws
    // differently per CPU
    for (int iStep = 0; iStep < 60; iStep++)
    {
        oController.update(1.0 / 60.0);
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vSettledPosition;
    Math::CVector3 vForwardAxis;

    QVERIFY(settledForwardAxis(oWorld, 0, vSettledPosition, vForwardAxis) == true);

    QKeyEvent cForwardPress(QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier);
    oController.keyPressEvent(&cForwardPress);

    QVERIFY(oController.driveForwardNorm() == 1.0);

    for (int iStep = 0; iStep < 300; iStep++)
    {
        oController.update(1.0 / 60.0);
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vFinalPosition;

    QVERIFY(oWorld.vehiclePosition(0, vFinalPosition) == true);

    Math::CVector3 vTravel = vFinalPosition - vSettledPosition;
    double dForwardM = vTravel.dot(vForwardAxis);
    double dLateralM = (vTravel - vForwardAxis * dForwardM).magnitude();

    QVERIFY2(dForwardM > 2.0, qPrintable(QString("Chassis moved %1 m forward, %2 m sideways").arg(dForwardM).arg(dLateralM)));
    QVERIFY2(dLateralM < 1.5, qPrintable(QString("Chassis drifted %1 m sideways").arg(dLateralM)));

    QVERIFY(pVehicle->summedForces_mss() == Math::CVector3(0.0, 0.0, 0.0));

    pVehicle->detachPhysicsVehicle();

    QVERIFY(pVehicle->hasPhysicsVehicle() == false);
    QVERIFY(oWorld.vehicleCount() == 0);

    oWorld.shutdown();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The hand brake is engaged by default and toggles with the H key.
*/
/***************************************************************************/
void CTestVehicle::testHandBrakeEngagedByDefault()
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    QVERIFY(pVehicle->isHandBrakeEngaged() == true);

    pVehicle->setHandBrakeEngaged(false);

    QVERIFY(pVehicle->isHandBrakeEngaged() == false);

    CForceController oController(nullptr);
    oController.setPositionTarget(pVehicle);

    QKeyEvent cTogglePress(QEvent::KeyPress, Qt::Key_H, Qt::NoModifier);
    oController.keyPressEvent(&cTogglePress);

    QVERIFY(pVehicle->isHandBrakeEngaged() == true);

    oController.keyPressEvent(&cTogglePress);

    QVERIFY(pVehicle->isHandBrakeEngaged() == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The hand brake flag loads from XML and reaches the physics wheels.
*/
/***************************************************************************/
void CTestVehicle::testHandBrakeReachesPhysicsWheels()
{
    QString sError;
    CXMLNode xComponent = CXMLNode::parseXML(QString("<Component Name=\"Wheel\" Class=\"CWheel\"><Position x=\"0.8\" y=\"0.4\" z=\"-3.0\" /><General Radius=\"0.8\" Width=\"0.5\" Driven=\"1\" HandBrake=\"1\" /></Component>"), &sError);

    QVERIFY2(sError.isEmpty() == true, qPrintable(sError));

    QSP<CVehicle> pVehicle(new CVehicle(nullptr));
    QSP<CWheel> pWheel(new CWheel(nullptr));

    pWheel->setParentName(QString("TestVehicle"));
    pWheel->loadParameters(QString(""), xComponent);

    QVERIFY(pWheel->handBrake() == true);

    pVehicle->childComponents().append(pWheel);

    QVector<CVehicleWheel> vPhysicsWheels = pVehicle->buildPhysicsWheels();

    QVERIFY(vPhysicsWheels.count() == 1);
    QVERIFY(vPhysicsWheels[0].m_bHandBrake == true);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Builds a 7500 kg vehicle with rear hand brake wheels.
*/
/***************************************************************************/
static QSP<CVehicle> createHeavyTestVehicle()
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    pVehicle->setMass_kg(7500.0);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QSP<CWheel> pWheel(new CWheel(nullptr));

        pWheel->setDriven(true);
        pWheel->setRadiusM(0.45);
        pWheel->setParentName(QString("TestVehicle"));
        pWheel->setPosition(Math::CVector3((iWheel % 2 == 0) ? 0.8 : -0.8, 0.45, (iWheel < 2) ? 1.5 : -1.5));

        if (iWheel < 2)
        {
            pWheel->setSteered(true);
        }
        else
        {
            pWheel->setHandBrake(true);
        }

        pVehicle->childComponents().append(pWheel);
    }

    return pVehicle;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Drives a heavy vehicle at full throttle, returns true and the travel
* along its settled forward axis with the sideways drift.
*/
static bool driveHeavyTestVehicle(bool bReleaseBrake, double& dForwardM, double& dLateralM)
{
    CPhysicsWorld oWorld;

    if (oWorld.initialize() == false)
    {
        return false;
    }

    if (oWorld.addStaticBox(Math::CVector3(50.0, 1.0, 50.0), Math::CVector3(0.0, -1.0, 0.0)) < 0)
    {
        return false;
    }

    QSP<CVehicle> pVehicle = createHeavyTestVehicle();

    // Note: origin starts at ground level (wheel center 0.45 minus radius
    // 0.45), the body center offset carries the chassis above it, a 1 cm
    // drop only, a high spawn free falls and tips
    const double dSpawnMarginM = 0.01;

    if (pVehicle->attachPhysicsVehicle(&oWorld, Math::CVector3(1.22, 1.425, 2.07), Math::CVector3(0.0, dSpawnMarginM, 0.0), Math::CQuaternion(0.0, 0.0, 0.0, 1.0), Math::CVector3(0.0, 1.425, 0.0)) == false)
    {
        return false;
    }

    if (bReleaseBrake == true)
    {
        pVehicle->setHandBrakeEngaged(false);
    }

    CForceController oController(nullptr);
    oController.setPositionTarget(pVehicle);

    // Note: settle at equilibrium before driving, a falling chassis yaws
    // differently per CPU
    for (int iStep = 0; iStep < 60; iStep++)
    {
        oController.update(1.0 / 60.0);
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vSettledPosition;
    Math::CVector3 vForwardAxis;

    if (settledForwardAxis(oWorld, 0, vSettledPosition, vForwardAxis) == false)
    {
        return false;
    }

    QKeyEvent cForwardPress(QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier);
    oController.keyPressEvent(&cForwardPress);

    for (int iStep = 0; iStep < 300; iStep++)
    {
        oController.update(1.0 / 60.0);
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vFinalPosition;

    if (oWorld.vehiclePosition(0, vFinalPosition) == false)
    {
        return false;
    }

    Math::CVector3 vTravel = vFinalPosition - vSettledPosition;

    dForwardM = vTravel.dot(vForwardAxis);
    dLateralM = (vTravel - vForwardAxis * dForwardM).magnitude();

    pVehicle->detachPhysicsVehicle();
    oWorld.shutdown();

    return true;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A released heavy vehicle drives, an engaged hand brake holds it back.
*/
/***************************************************************************/
void CTestVehicle::testHeavyVehicleDrivesOnlyWhenReleased()
{
    double dReleasedForwardM = 0.0;
    double dReleasedLateralM = 0.0;

    QVERIFY(driveHeavyTestVehicle(true, dReleasedForwardM, dReleasedLateralM) == true);
    QVERIFY2(dReleasedForwardM > 3.0, qPrintable(QString("Released heavy vehicle covered %1 m forward, %2 m sideways").arg(dReleasedForwardM).arg(dReleasedLateralM)));

    double dEngagedForwardM = 0.0;
    double dEngagedLateralM = 0.0;

    QVERIFY(driveHeavyTestVehicle(false, dEngagedForwardM, dEngagedLateralM) == true);

    // Note: a held chassis barely moves, its travel magnitude stays positive
    // by construction, no sign assumption on a jitter around zero
    double dEngagedTravelM = sqrt(dEngagedForwardM * dEngagedForwardM + dEngagedLateralM * dEngagedLateralM);

    QVERIFY2(dEngagedTravelM < 0.3 * dReleasedForwardM, qPrintable(QString("Engaged %1 m versus released %2 m").arg(dEngagedTravelM).arg(dReleasedForwardM)));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief All wheels of a settled heavy vehicle report ground contact.
*/
/***************************************************************************/
void CTestVehicle::testHeavyVehicleWheelsTouchGround()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);
    QVERIFY(oWorld.addStaticBox(Math::CVector3(50.0, 1.0, 50.0), Math::CVector3(0.0, -1.0, 0.0)) >= 0);

    QSP<CVehicle> pVehicle = createHeavyTestVehicle();

    QVERIFY(pVehicle->attachPhysicsVehicle(&oWorld, Math::CVector3(1.22, 1.425, 2.07), Math::CVector3(0.0, 2.0, 0.0), Math::CQuaternion(0.0, 0.0, 0.0, 1.0), Math::CVector3(0.0, 1.425, 0.0)) == true);

    for (int iStep = 0; iStep < 120; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QVERIFY2(oWorld.vehicleWheelHasContact(0, iWheel) == true, qPrintable(QString("Wheel %1 has no contact").arg(iWheel)));
    }

    pVehicle->detachPhysicsVehicle();
    oWorld.shutdown();
}

//-------------------------------------------------------------------------------------------------
