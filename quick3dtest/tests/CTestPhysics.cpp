
// Application
#include "CTestPhysics.h"
#include "CPhysicsWorld.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Creates a floor and a four wheel drive chassis above it.
*/
/***************************************************************************/
static int createFourWheelVehicle(CPhysicsWorld& oWorld)
{
    int iFloor = oWorld.addStaticBox(Math::CVector3(50.0, 1.0, 50.0), Math::CVector3(0.0, -1.0, 0.0));

    if (iFloor < 0)
    {
        return -1;
    }

    QVector<CVehicleWheel> vWheels;

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        CVehicleWheel cWheel;
        cWheel.m_vPosition = Math::CVector3((iWheel % 2 == 0) ? 0.9 : -0.9, -0.27, (iWheel < 2) ? 1.2 : -1.2);
        cWheel.m_bDriven = true;

        if (iWheel < 2)
        {
            cWheel.m_dMaxSteerAngleRad = 0.5;
        }

        vWheels.append(cWheel);
    }

    // Note: body starts at suspension equilibrium (radius 0.4 minus wheel
    // center -0.27) plus a small margin, a high spawn free falls and tips
    // the chassis differently per CPU, which drove the chassis backwards
    const double dEquilibriumYM = 0.67;
    const double dSpawnMarginM = 0.23;

    return oWorld.addVehicle(Math::CVector3(0.9, 0.3, 2.0), 1200.0, Math::CVector3(0.0, dEquilibriumYM + dSpawnMarginM, 0.0), vWheels);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestPhysics::CTestPhysics()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Placed bodies read back at their placement through the float conversion.
*/
/***************************************************************************/
void CTestPhysics::testBodyPositionRoundTripsPlacement()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    Math::CVector3 vPlaced(1.5, 5.0, -2.5);

    int iSphere = oWorld.addDynamicSphere(0.5, vPlaced);

    QVERIFY(iSphere >= 0);

    Math::CVector3 vRead;

    QVERIFY(oWorld.bodyPosition(iSphere, vRead) == true);
    QVERIFY(qAbs(vRead.X - vPlaced.X) < 0.0001);
    QVERIFY(qAbs(vRead.Y - vPlaced.Y) < 0.0001);
    QVERIFY(qAbs(vRead.Z - vPlaced.Z) < 0.0001);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A dynamic sphere dropped on a static floor comes to rest on the floor.
*/
/***************************************************************************/
void CTestPhysics::testSphereFallsOntoStaticFloor()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    int iFloor = oWorld.addStaticBox(Math::CVector3(50.0, 1.0, 50.0), Math::CVector3(0.0, -1.0, 0.0));

    QVERIFY(iFloor >= 0);

    int iSphere = oWorld.addDynamicSphere(0.5, Math::CVector3(0.0, 5.0, 0.0));

    QVERIFY(iSphere >= 0);
    QVERIFY(oWorld.bodyCount() == 2);

    for (int iStep = 0; iStep < 180; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vRest;

    QVERIFY(oWorld.bodyPosition(iSphere, vRest) == true);
    QVERIFY(qAbs(vRest.X) < 0.05);
    QVERIFY(qAbs(vRest.Z) < 0.05);
    QVERIFY(qAbs(vRest.Y - 0.5) < 0.05);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Unknown body indexes report failure instead of crashing.
*/
/***************************************************************************/
void CTestPhysics::testUnknownBodyReportsFailure()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    Math::CVector3 vPosition;

    QVERIFY(oWorld.bodyPosition(-1, vPosition) == false);
    QVERIFY(oWorld.bodyPosition(0, vPosition) == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A chassis dropped on its wheels settles on the suspension travel. \br\br
* The declared wheel position is the wheel center at mid travel, the Jolt
* hardpoint sits one mid travel above it, so the body rests near radius
* minus the declared center height plus the compressed travel.
*/
/***************************************************************************/
void CTestPhysics::testVehicleSettlesOnSuspension()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    int iVehicle = createFourWheelVehicle(oWorld);

    QVERIFY(iVehicle >= 0);
    QVERIFY(oWorld.vehicleCount() == 1);

    for (int iStep = 0; iStep < 180; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vRest;

    QVERIFY(oWorld.vehiclePosition(iVehicle, vRest) == true);
    QVERIFY(vRest.Y > 0.4 && vRest.Y < 1.0);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QVERIFY(oWorld.vehicleWheelHasContact(iVehicle, iWheel) == true);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A driven chassis moves forward along its local +Z axis.
*/
/***************************************************************************/
void CTestPhysics::testVehicleDrivesForward()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    int iVehicle = createFourWheelVehicle(oWorld);

    QVERIFY(iVehicle >= 0);

    // Note: settle at equilibrium before driving, a falling chassis bounces
    // and yaws differently per CPU
    QVERIFY(oWorld.setVehicleDriverInput(iVehicle, 0.0, 0.0, 0.0, 0.0) == true);

    for (int iStep = 0; iStep < 60; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    QVERIFY(oWorld.setVehicleDriverInput(iVehicle, 1.0, 0.0, 0.0, 0.0) == true);

    for (int iStep = 0; iStep < 300; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vPosition;

    QVERIFY(oWorld.vehiclePosition(iVehicle, vPosition) == true);
    QVERIFY2(vPosition.Z > 2.0, qPrintable(QString("Chassis moved to z=%1").arg(vPosition.Z)));
    QVERIFY(qAbs(vPosition.X) < 1.5);
    QVERIFY(vPosition.Y > 0.3 && vPosition.Y < 1.6);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Creates a VBCI like heavy chassis above the given start position.
*/
/***************************************************************************/
static int createHeavyVehicle(CPhysicsWorld& oWorld, const Math::CVector3& vStart)
{
    QVector<CVehicleWheel> vWheels;

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        CVehicleWheel cWheel;
        cWheel.m_vPosition = Math::CVector3((iWheel % 2 == 0) ? 0.8 : -0.8, -0.4, (iWheel < 2) ? 3.0 : -3.0);
        cWheel.m_dRadiusM = 0.8;
        cWheel.m_dWidthM = 0.5;
        cWheel.m_bDriven = true;
        cWheel.m_bHandBrake = (iWheel >= 2);

        vWheels.append(cWheel);
    }

    return oWorld.addVehicle(Math::CVector3(1.3, 0.5, 3.1), 26000.0, vStart, vWheels);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A heavy chassis at full throttle climbs a 30 percent grade.
*/
/***************************************************************************/
void CTestPhysics::testHeavyVehicleClimbsSlope()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    const int iPoints = 32;
    const float fSpacingM = 2.0f;
    const float fGrade = 0.3f;
    const float fHalfM = 0.5f * float(iPoints - 1) * fSpacingM;

    QVector<float> vGrid;

    for (int iRow = 0; iRow < iPoints; iRow++)
    {
        float fHeightM = (fHalfM - float(iRow) * fSpacingM) * fGrade;

        for (int iCol = 0; iCol < iPoints; iCol++)
        {
            Q_UNUSED(iCol);
            vGrid.append(fHeightM);
        }
    }

    QVERIFY(oWorld.addHeightfield(vGrid, iPoints, fSpacingM, fSpacingM, Math::CVector3(0.0, 0.0, 0.0), Math::CMatrix4()) >= 0);

    const double dStartZM = -10.0;
    const double dGroundM = dStartZM * double(fGrade);

    // Note: body starts at suspension equilibrium (radius 0.8 minus wheel
    // center -0.4) above the local ground, a high spawn free falls and tips
    const double dEquilibriumYM = 1.2;

    int iVehicle = createHeavyVehicle(oWorld, Math::CVector3(0.0, dGroundM + dEquilibriumYM, dStartZM));

    QVERIFY(iVehicle >= 0);

    QVERIFY(oWorld.setVehicleDriverInput(iVehicle, 0.0, 0.0, 0.0, 1.0) == true);

    for (int iStep = 0; iStep < 120; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vSettled;

    QVERIFY(oWorld.vehiclePosition(iVehicle, vSettled) == true);

    QVERIFY(oWorld.setVehicleDriverInput(iVehicle, 1.0, 0.0, 0.0, 0.0) == true);

    // Note: 300 steps stay on the 64 m grid (start -10, climb to about +5),
    // 600 steps drive off the north edge and fall, which fails the climb
    for (int iStep = 0; iStep < 300; iStep++)
    {
        oWorld.step(1.0 / 60.0);
    }

    Math::CVector3 vFinal;

    QVERIFY(oWorld.vehiclePosition(iVehicle, vFinal) == true);
    QVERIFY2(qAbs(vFinal.X) < 2.5, qPrintable(QString("X drifted to %1").arg(vFinal.X)));
    QVERIFY2(vFinal.Z - vSettled.Z > 4.0, qPrintable(QString("Z %1 to %2").arg(vSettled.Z).arg(vFinal.Z)));
    QVERIFY2(vFinal.Y - vSettled.Y > 0.8, qPrintable(QString("Y %1 to %2 settled Z %3 final Z %4").arg(vSettled.Y).arg(vFinal.Y).arg(vSettled.Z).arg(vFinal.Z)));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Unknown vehicle and wheel indexes report failure instead of crashing.
*/
/***************************************************************************/
void CTestPhysics::testUnknownVehicleReportsFailure()
{
    CPhysicsWorld oWorld;

    QVERIFY(oWorld.initialize() == true);

    int iVehicle = createFourWheelVehicle(oWorld);

    QVERIFY(iVehicle >= 0);

    Math::CVector3 vPosition;

    QVERIFY(oWorld.vehiclePosition(-1, vPosition) == false);
    QVERIFY(oWorld.vehiclePosition(1, vPosition) == false);
    QVERIFY(oWorld.setVehicleDriverInput(-1, 1.0, 0.0, 0.0, 0.0) == false);
    QVERIFY(oWorld.vehicleWheelHasContact(-1, 0) == false);
    QVERIFY(oWorld.vehicleWheelHasContact(iVehicle, -1) == false);
    QVERIFY(oWorld.vehicleWheelHasContact(iVehicle, 4) == false);
}

//-------------------------------------------------------------------------------------------------
