
// Jolt (Jolt.h first, required by the library)
#include <Jolt/Jolt.h>
#include <Jolt/Math/Quat.h>

// Qt
#include <QtMath>
#include <QKeyEvent>

// Application
#include "CTestScene.h"
#include "C3DScene.h"
#include "CSunlightModel.h"
#include "CAtmosphereOptics.h"
#include "CVehicle.h"
#include "CWheel.h"
#include "CBoundedMeshInstances.h"
#include "CPhysicsMath.h"
#include "CGeoFrame.h"
#include "internal/CPhysicsJolt.h"
#include "CWorldChunk.h"
#include "CTerrain.h"
#include "CStandardController.h"
#include "CForceController.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestScene::CTestScene()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Builds a scene vehicle with four driven wheels and mesh bounds.
*/
/***************************************************************************/
static QSP<CVehicle> createSceneTestVehicle(const CGeoloc& gPosition)
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    pVehicle->setMass_kg(1200.0);
    pVehicle->setGeoloc(gPosition);

    QSP<CBoundedMeshInstances> pMesh(new CBoundedMeshInstances(nullptr));
    pMesh->setBounds(CBoundingBox(Math::CVector3(-1.3, 0.0, -3.1), Math::CVector3(1.3, 2.6, 3.1)));
    pMesh->setParentName(QString("SceneTestVehicle"));
    pVehicle->childComponents().append(pMesh);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QSP<CWheel> pWheel(new CWheel(nullptr));

        pWheel->setDriven(true);

        // Note: the loader names the parent, which switches position() and
        // setPosition() to the local frame. setParent() is avoided, it would
        // wrap this in a second ownership chain. The name must be set before
        // the position, setPosition on a root object writes the geolocation.
        pWheel->setParentName(QString("SceneTestVehicle"));
        pWheel->setPosition(Math::CVector3((iWheel % 2 == 0) ? 0.9 : -0.9, -0.27, (iWheel < 2) ? 1.2 : -1.2));

        pVehicle->childComponents().append(pWheel);
    }

    return pVehicle;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Builds a VBCI like heavy vehicle with low axles and large tires.
*/
/***************************************************************************/
static QSP<CVehicle> createHeavyTestVehicle(const CGeoloc& gPosition)
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    pVehicle->setMass_kg(26000.0);
    pVehicle->setGeoloc(gPosition);

    QSP<CBoundedMeshInstances> pMesh(new CBoundedMeshInstances(nullptr));
    pMesh->setBounds(CBoundingBox(Math::CVector3(-1.3, 0.0, -3.2), Math::CVector3(1.3, 2.6, 3.1)));
    pMesh->setParentName(QString("HeavyTestVehicle"));
    pVehicle->childComponents().append(pMesh);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QSP<CWheel> pWheel(new CWheel(nullptr));

        pWheel->setDriven(true);
        pWheel->setRadiusM(0.8);
        pWheel->setWidthM(0.5);

        // Note: see createSceneTestVehicle for the parent name workaround
        pWheel->setParentName(QString("HeavyTestVehicle"));
        pWheel->setPosition(Math::CVector3((iWheel % 2 == 0) ? 0.8 : -0.8, 0.4, (iWheel < 2) ? 3.0 : -3.0));

        if (iWheel < 2)
        {
            pWheel->setSteered(true);
        }

        pVehicle->childComponents().append(pWheel);
    }

    return pVehicle;
}

/***************************************************************************/
/*!
* \brief Builds a PVP like vehicle with rear hand brake wheels and a force controller child.
*/
/***************************************************************************/
static QSP<CVehicle> createPvpLikeVehicle(const CGeoloc& gPosition)
{
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));

    // Note: mass, bounds, wheels and speed cap replicate the Visitor2 entry
    // of Quick3DTest.xml, the controller child replicates its Controller entry
    pVehicle->setMass_kg(7500.0);
    pVehicle->setSpeedMS(4.0);
    pVehicle->setGeoloc(gPosition);

    QSP<CBoundedMeshInstances> pMesh(new CBoundedMeshInstances(nullptr));
    pMesh->setBounds(CBoundingBox(Math::CVector3(-1.33, 0.0, -2.1312), Math::CVector3(1.0982, 2.85, 2.0001)));
    pMesh->setParentName(QString("PvpLikeVehicle"));
    pVehicle->childComponents().append(pMesh);

    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QSP<CWheel> pWheel(new CWheel(nullptr));

        pWheel->setDriven(true);
        pWheel->setRadiusM(0.45);

        // Note: see createSceneTestVehicle for the parent name workaround
        pWheel->setParentName(QString("PvpLikeVehicle"));
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

    QSP<CForceController> pController(new CForceController(nullptr));
    pController->setParentName(QString("PvpLikeVehicle"));
    pVehicle->childComponents().append(pController);

    return pVehicle;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Registers a flat physics grid under \a gCenter at \a dAltitude.
*/
/***************************************************************************/
static int registerFlatTestGrid(C3DScene& cScene, const CGeoloc& gCenter, double dAltitude)
{
    const int iPoints = 5;

    QVector<float> vGrid;

    for (int iSample = 0; iSample < iPoints * iPoints; iSample++)
    {
        vGrid.append(float(dAltitude));
    }

    CGeoloc gDatum(gCenter.Latitude, gCenter.Longitude, 0.0);

    SGeoFrame cFrame = CGeoFrame::heightfieldFrame(gDatum, CGeoloc(0.01, 0.01, 0.0), iPoints, cScene.physicsReference().toVector3());

    return cScene.physicsWorld()->addHeightfield(vGrid, iPoints, cFrame.SpacingXM, cFrame.SpacingZM, cFrame.Position, cFrame.Rotation);
}

/***************************************************************************/
/*!
* \brief Adds an unattached test vehicle at \a dStartAltitudeM.
*/
/***************************************************************************/
static QSP<CVehicle> addCandidateVehicle(C3DScene& cScene, double dStartAltitudeM)
{
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(19.237738, 10.896701, dStartAltitudeM));
    cScene.addComponent(pVehicle);

    cScene.ensurePhysicsReference(pVehicle->geoloc());

    return pVehicle;
}

/***************************************************************************/
/*!
* \brief Registers a flat grid at \a dGroundAltitudeM and attaches \a pVehicle.
*/
/***************************************************************************/
static bool attachOverNewFlatGrid(C3DScene& cScene, QSP<CVehicle>& pVehicle, double dGroundAltitudeM)
{
    if (pVehicle == nullptr)
    {
        return false;
    }

    if (registerFlatTestGrid(cScene, pVehicle->geoloc(), dGroundAltitudeM) < 0)
    {
        return false;
    }

    cScene.ensureVehiclePhysics();

    return true;
}

/***************************************************************************/
/*!
* \brief Attaches a test vehicle over a flat grid at \a dGroundAltitude.
*/
/***************************************************************************/
static bool ensureVehicleOverFlatGrid(C3DScene& cScene, double dStartAltitudeM, double dGroundAltitudeM, QSP<CVehicle>& pVehicle)
{
    pVehicle = addCandidateVehicle(cScene, dStartAltitudeM);

    return attachOverNewFlatGrid(cScene, pVehicle, dGroundAltitudeM);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Matrix to quaternion conversion inverts CMatrix4::fromQuaternion exactly.
*/
/***************************************************************************/
void CTestScene::testQuaternionRoundTripsRotationMatrix()
{
    Math::CVector3 vEuler(0.3, 1.2, -0.2);
    Math::CMatrix4 mRotation = Math::CMatrix4::makeRotation(vEuler);
    Math::CQuaternion cQuaternion = CPhysicsMath::toQuaternion(mRotation);
    Math::CMatrix4 mRoundTripped = Math::CMatrix4::fromQuaternion(cQuaternion);

    QVector<Math::CVector3> vBasis;
    vBasis.append(Math::CVector3(1.0, 0.0, 0.0));
    vBasis.append(Math::CVector3(0.0, 1.0, 0.0));
    vBasis.append(Math::CVector3(0.0, 0.0, 1.0));

    foreach (Math::CVector3 vDirection, vBasis)
    {
        Math::CVector3 vExpected = mRotation * vDirection;
        Math::CVector3 vActual = mRoundTripped * vDirection;

        QVERIFY2(qAbs(vActual.X - vExpected.X) < 1.0e-9, qPrintable(QString("X differs: %1 vs %2").arg(vActual.X).arg(vExpected.X)));
        QVERIFY2(qAbs(vActual.Y - vExpected.Y) < 1.0e-9, qPrintable(QString("Y differs: %1 vs %2").arg(vActual.Y).arg(vExpected.Y)));
        QVERIFY2(qAbs(vActual.Z - vExpected.Z) < 1.0e-9, qPrintable(QString("Z differs: %1 vs %2").arg(vActual.Z).arg(vExpected.Z)));
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Converted rotations turn vectors like Jolt quaternions do.
*/
/***************************************************************************/
void CTestScene::testConvertedRotationMatchesJolt()
{
    Math::CVector3 vEuler(0.0, Math::Angles::toRad(90.0), 0.0);
    Math::CMatrix4 mRotation = Math::CMatrix4::makeRotation(vEuler);
    JPH::Quat cJoltRotation = CPhysicsJolt::toJoltRotation(CPhysicsMath::toQuaternion(mRotation)).Normalized();

    JPH::Vec3 vTurned = cJoltRotation * JPH::Vec3(0.0f, 0.0f, 1.0f);
    Math::CVector3 vExpected = mRotation * Math::CVector3(0.0, 0.0, 1.0);

    QVERIFY2(qAbs(double(vTurned.GetX()) - vExpected.X) < 1.0e-6, "Jolt X differs from engine matrix");
    QVERIFY2(qAbs(double(vTurned.GetY()) - vExpected.Y) < 1.0e-6, "Jolt Y differs from engine matrix");
    QVERIFY2(qAbs(double(vTurned.GetZ()) - vExpected.Z) < 1.0e-6, "Jolt Z differs from engine matrix");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief fromBasisVectors maps local axes to the basis in both engines.
*/
/***************************************************************************/
void CTestScene::testBasisVectorsMapLocalAxes()
{
    Math::CAxis aAxis = CGeoloc(19.2, 10.9, 0.0).getTopocentricAxis();
    Math::CMatrix4 mBasis = Math::CMatrix4::fromBasisVectors(aAxis.Right, aAxis.Up, aAxis.Front);

    // Note: the engine operator applies the transposed matrix, columns hold
    // the basis images, any hand written layout regresses here first
    QVector<Math::CVector3> vLocal;
    vLocal.append(Math::CVector3(1.0, 0.0, 0.0));
    vLocal.append(Math::CVector3(0.0, 1.0, 0.0));
    vLocal.append(Math::CVector3(0.0, 0.0, 1.0));

    QVector<Math::CVector3> vWorld;
    vWorld.append(aAxis.Right);
    vWorld.append(aAxis.Up);
    vWorld.append(aAxis.Front);

    for (int iAxis = 0; iAxis < 3; iAxis++)
    {
        Math::CVector3 vMapped = mBasis * vLocal[iAxis];

        QVERIFY2(qAbs(vMapped.X - vWorld[iAxis].X) < 1.0e-9, "Basis X differs");
        QVERIFY2(qAbs(vMapped.Y - vWorld[iAxis].Y) < 1.0e-9, "Basis Y differs");
        QVERIFY2(qAbs(vMapped.Z - vWorld[iAxis].Z) < 1.0e-9, "Basis Z differs");
    }

    JPH::Quat cRotation = CPhysicsJolt::toJoltRotation(CPhysicsMath::toQuaternion(mBasis)).Normalized();

    JPH::Vec3 vEast = cRotation * JPH::Vec3(1.0f, 0.0f, 0.0f);
    JPH::Vec3 vUp = cRotation * JPH::Vec3(0.0f, 1.0f, 0.0f);
    JPH::Vec3 vNorth = cRotation * JPH::Vec3(0.0f, 0.0f, 1.0f);

    QVERIFY2((vEast - JPH::Vec3(float(aAxis.Right.X), float(aAxis.Right.Y), float(aAxis.Right.Z))).Length() < 1.0e-5f, "Jolt east differs");
    QVERIFY2((vUp - JPH::Vec3(float(aAxis.Up.X), float(aAxis.Up.Y), float(aAxis.Up.Z))).Length() < 1.0e-5f, "Jolt up differs");
    QVERIFY2((vNorth - JPH::Vec3(float(aAxis.Front.X), float(aAxis.Front.Y), float(aAxis.Front.Z))).Length() < 1.0e-5f, "Jolt north differs");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Attaching a wheel less vehicle fails without freezing a reference.
*/
/***************************************************************************/
void CTestScene::testAttachRefusesWheelLessVehicle()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle(new CVehicle(nullptr));
    pVehicle->setGeoloc(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pVehicle);

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == false);
    QVERIFY(pVehicle->hasPhysicsVehicle() == false);
    QVERIFY(cScene.hasPhysicsReference() == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Attaching freezes the physics reference on the vehicle geoloc.
*/
/***************************************************************************/
void CTestScene::testAttachFreezesPhysicsReference()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pVehicle);

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);
    QVERIFY(pVehicle->hasPhysicsVehicle() == true);
    QVERIFY(cScene.hasPhysicsReference() == true);
    QVERIFY(cScene.physicsReference().Latitude == 48.0);
    QVERIFY(cScene.physicsReference().Longitude == 2.0);
    QVERIFY(cScene.physicsReference().Altitude == 100.0);

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Attaching aligns gravity with the frozen reference down direction.
*/
/***************************************************************************/
void CTestScene::testAttachAlignsGravityWithReferenceDown()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(19.237738, 10.896701, 42.0));
    cScene.addComponent(pVehicle);

    // Note: terrain freezes the reference before the first vehicle attach
    cScene.ensurePhysicsReference(pVehicle->geoloc());

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);

    Math::CVector3 vExpected = cScene.physicsReference().getTopocentricAxis().Up * -9.81;
    Math::CVector3 vGravity = cScene.physicsWorld()->gravity();

    QVERIFY2((vGravity - vExpected).magnitude() < 1.0e-4, qPrintable(QString("Unexpected gravity %1 %2 %3").arg(vGravity.X).arg(vGravity.Y).arg(vGravity.Z)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Attaching aligns gravity with the vehicle down direction.
*/
 /***************************************************************************/
void CTestScene::testAttachAlignsGravityWithVehicleDown()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createHeavyTestVehicle(CGeoloc(19.237738, 10.896701, 42.0));
    cScene.addComponent(pVehicle);

    // Note: the world terrain root can freeze the reference far from gameplay,
    // gravity must follow the vehicle down direction, not that distant point
    cScene.ensurePhysicsReference(CGeoloc(0.0, 0.0, 42.0));

    QVERIFY(registerFlatTestGrid(cScene, pVehicle->geoloc(), 42.0) >= 0);
    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);

    Math::CVector3 vExpected = pVehicle->geoloc().getTopocentricAxis().Up * -9.81;
    Math::CVector3 vGravity = cScene.physicsWorld()->gravity();

    QVERIFY2((vGravity - vExpected).magnitude() < 1.0e-4, qPrintable(QString("Unexpected gravity %1 %2 %3").arg(vGravity.X).arg(vGravity.Y).arg(vGravity.Z)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Stepped attached vehicles fall along the reference down direction.
*/
/***************************************************************************/
void CTestScene::testSceneStepsAttachedVehicle()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pVehicle);

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);

    for (int iStep = 0; iStep < 60; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);
    }

    QVERIFY(cScene.physicsSteps() == 60);

    // Note: free fall drops 4.9 meters in one second, the chassis center offset
    // and the suspension droop add less than a meter of slack
    double dAltitude = pVehicle->geoloc().Altitude;
    QVERIFY2(dAltitude > 90.0 && dAltitude < 98.0, qPrintable(QString("Unexpected altitude %1").arg(dAltitude)));

    // Note: gravity pulls along the reference down direction, the topocentric
    // velocity stays vertical
    Math::CVector3 vVelocity = pVehicle->velocity_ms();
    QVERIFY2(vVelocity.Y > -11.0 && vVelocity.Y < -8.5, qPrintable(QString("Unexpected vertical speed %1").arg(vVelocity.Y)));
    QVERIFY2(qAbs(vVelocity.X) < 0.5, qPrintable(QString("Unexpected lateral speed %1").arg(vVelocity.X)));
    QVERIFY2(qAbs(vVelocity.Z) < 0.5, qPrintable(QString("Unexpected longitudinal speed %1").arg(vVelocity.Z)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A large frame delta executes at most four fixed steps.
*/
/***************************************************************************/
void CTestScene::testFixedStepCapsPerFrame()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pVehicle);

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);

    cScene.updateScene(1.0);

    QVERIFY(cScene.physicsSteps() == 4);

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Detaching removes the vehicle from the world.
*/
/***************************************************************************/
void CTestScene::testDetachRemovesVehicle()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pVehicle);

    QVERIFY(cScene.attachVehicleToPhysics(pVehicle) == true);
    QVERIFY(cScene.physicsWorld()->vehicleCount() == 1);

    cScene.detachVehicleFromPhysics(pVehicle);

    QVERIFY(pVehicle->hasPhysicsVehicle() == false);
    QVERIFY(cScene.physicsWorld()->vehicleCount() == 0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A ray hits a registered height field grid at the grid altitude.
*/
/***************************************************************************/
void CTestScene::testHeightfieldRayHitsRegisteredGrid()
{
    CPhysicsWorld cWorld;
    QVERIFY(cWorld.initialize() == true);

    CGeoloc gCenter(19.2, 10.9, 0.0);
    Math::CVector3 vFrameOrigin = CGeoloc(19.2, 10.9, 100.0).toVector3();
    Math::CVector3 vUp = gCenter.getTopocentricAxis().Up;
    Math::CVector3 vCenter = gCenter.toVector3() - vFrameOrigin;

    double dHitDistanceM = 0.0;

    // Note: control case, the box top plane is crossed 1/(Up.Y) down the
    // tilted ray after the 8 meter vertical drop to the box top
    int iBox = cWorld.addStaticBox(Math::CVector3(10.0, 1.0, 10.0), vCenter + vUp * 42.0);

    QVERIFY(iBox >= 0);

    double dBoxHitM = 8.0 - 1.0 / vUp.Y;

    QVERIFY(cWorld.castRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == true);
    QVERIFY2(qAbs(dHitDistanceM - dBoxHitM) < 1.0e-3, qPrintable(QString("Unexpected box hit distance %1").arg(dHitDistanceM)));

    // Note: the terrain query sees height fields only, the static box alone
    // is not terrain
    QVERIFY(cWorld.castTerrainRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == false);

    // Note: the box would hide the grid below it, drop all bodies first
    cWorld.clear();

    const int iPoints = 5;

    QVector<float> vGrid;

    for (int iSample = 0; iSample < iPoints * iPoints; iSample++)
    {
        vGrid.append(42.0f);
    }

    SGeoFrame cFrame = CGeoFrame::heightfieldFrame(gCenter, CGeoloc(0.01, 0.01, 0.0), iPoints, vFrameOrigin);

    QVERIFY(cFrame.SpacingXM > 0.0f);
    QVERIFY(cFrame.SpacingZM > 0.0f);

    // Note: the converted frame turns local axes into the topocentric basis
    JPH::Quat cFrameRotation = CPhysicsJolt::toJoltRotation(CPhysicsMath::toQuaternion(cFrame.Rotation)).Normalized();
    Math::CAxis aAxis = gCenter.getTopocentricAxis();

    JPH::Vec3 vEast = cFrameRotation * JPH::Vec3(1.0f, 0.0f, 0.0f);
    JPH::Vec3 vUpFrame = cFrameRotation * JPH::Vec3(0.0f, 1.0f, 0.0f);
    JPH::Vec3 vNorth = cFrameRotation * JPH::Vec3(0.0f, 0.0f, 1.0f);

    QVERIFY2((vEast - JPH::Vec3(float(aAxis.Right.X), float(aAxis.Right.Y), float(aAxis.Right.Z))).Length() < 1.0e-5f, "Frame east differs");
    QVERIFY2((vUpFrame - JPH::Vec3(float(aAxis.Up.X), float(aAxis.Up.Y), float(aAxis.Up.Z))).Length() < 1.0e-5f, "Frame up differs");
    QVERIFY2((vNorth - JPH::Vec3(float(aAxis.Front.X), float(aAxis.Front.Y), float(aAxis.Front.Z))).Length() < 1.0e-5f, "Frame north differs");

    int iHeightfield = cWorld.addHeightfield(vGrid, iPoints, cFrame.SpacingXM, cFrame.SpacingZM, cFrame.Position, cFrame.Rotation);

    QVERIFY(iHeightfield >= 0);
    QVERIFY(cWorld.heightfieldCount() == 1);

    // Note: the ray starts 50 meters above the grid center, the flat grid
    // holds 42 meters, the hit lands 8 meters down the ray
    QVERIFY(cWorld.castRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == true);
    QVERIFY2(qAbs(dHitDistanceM - 8.0) < 0.1, qPrintable(QString("Unexpected hit distance %1").arg(dHitDistanceM)));
    QVERIFY(cWorld.castTerrainRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == true);
    QVERIFY2(qAbs(dHitDistanceM - 8.0) < 0.1, qPrintable(QString("Unexpected terrain hit distance %1").arg(dHitDistanceM)));

    QVERIFY(cWorld.removeHeightfield(iHeightfield) == true);
    QVERIFY(cWorld.heightfieldCount() == 0);
    QVERIFY(cWorld.castTerrainRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == false);
    QVERIFY(cWorld.castRay(vCenter + vUp * 50.0, vUp * -1.0, 200.0, &dHitDistanceM) == false);

    cWorld.shutdown();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A ready terrain mesh copies to a north first altitude grid.
*/
/***************************************************************************/
void CTestScene::testChunkTerrainGridCopiesReadyMesh()
{
    C3DScene cScene(false);

    QSP<CTerrain> pTerrain(new CTerrain(&cScene, nullptr, CGeoloc(19.2, 10.9, 0.0), CGeoloc(0.01, 0.01, 0.0), CGeoloc(19.2, 10.9, 0.0), CGeoloc(0.01, 0.01, 0.0), 5, 0, 4, false, true));

    QVERIFY(pTerrain->isOK() == true);

    QSP<CWorldChunk> pChunk(new CWorldChunk(&cScene, nullptr, nullptr));
    pChunk->setTerrain(pTerrain, false);

    QVector<float> vGrid;
    int iPoints = 0;

    QVERIFY(pChunk->terrainGrid(vGrid, iPoints) == true);
    QVERIFY(iPoints == 5);
    QVERIFY(vGrid.count() == 25);

    // Note: no height source was given, every mesh altitude defaults to zero
    foreach (float fHeight, vGrid)
    {
        QVERIFY(fHeight == 0.0f);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief ensureVehiclePhysics attaches a vehicle over physics ground only.
*/
/***************************************************************************/
void CTestScene::testEnsureAttachesVehicleOverPhysicsGround()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle = createSceneTestVehicle(CGeoloc(19.237738, 10.896701, 42.0));
    cScene.addComponent(pVehicle);

    cScene.ensurePhysicsReference(pVehicle->geoloc());

    const int iPoints = 5;

    QVector<float> vGrid;

    for (int iSample = 0; iSample < iPoints * iPoints; iSample++)
    {
        vGrid.append(42.0f);
    }

    CGeoloc gCenter(19.237738, 10.896701, 0.0);

    SGeoFrame cFrame = CGeoFrame::heightfieldFrame(gCenter, CGeoloc(0.01, 0.01, 0.0), iPoints, cScene.physicsReference().toVector3());

    int iHeightfield = cScene.physicsWorld()->addHeightfield(vGrid, iPoints, cFrame.SpacingXM, cFrame.SpacingZM, cFrame.Position, cFrame.Rotation);

    QVERIFY(iHeightfield >= 0);

    cScene.ensureVehiclePhysics();

    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    // Note: no physics ground exists under the far vehicle, it keeps the arcade path
    QSP<CVehicle> pFarVehicle = createSceneTestVehicle(CGeoloc(48.0, 2.0, 100.0));
    cScene.addComponent(pFarVehicle);

    cScene.ensureVehiclePhysics();

    QVERIFY(pFarVehicle->hasPhysicsVehicle() == false);

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A close spawn snaps to the measured terrain before attaching.
*/
/***************************************************************************/
void CTestScene::testEnsureSnapsCloseSpawnToMeasuredGround()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle;

    QVERIFY(ensureVehicleOverFlatGrid(cScene, 50.0, 42.0, pVehicle) == true);
    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    // Note: 50 is below 42 + 20, the authored pose moves to the ground first
    QVERIFY2(qAbs(pVehicle->geoloc().Altitude - 42.0) < 0.01, qPrintable(QString("Unexpected snapped altitude %1").arg(pVehicle->geoloc().Altitude)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A vehicle stays detached until late terrain appears, then snaps.
*/
/***************************************************************************/
void CTestScene::testEnsureAttachesWhenTerrainAppearsLate()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle = addCandidateVehicle(cScene, 50.0);

    QVERIFY(pVehicle != nullptr);

    // Note: no height field exists yet, the candidate keeps the arcade path
    cScene.ensureVehiclePhysics();
    QVERIFY(pVehicle->hasPhysicsVehicle() == false);

    // Note: the terrain arrives afterward, the close spawn snaps to 42 first
    QVERIFY(attachOverNewFlatGrid(cScene, pVehicle, 42.0) == true);
    QVERIFY(pVehicle->hasPhysicsVehicle() == true);
    QVERIFY2(qAbs(pVehicle->geoloc().Altitude - 42.0) < 0.01, qPrintable(QString("Unexpected snapped altitude %1").arg(pVehicle->geoloc().Altitude)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A high spawn keeps its authored altitude instead of teleporting down.
*/
/***************************************************************************/
void CTestScene::testEnsureKeepsHighSpawnAltitude()
{
    C3DScene cScene(false);
    QSP<CVehicle> pVehicle;

    QVERIFY(ensureVehicleOverFlatGrid(cScene, 70.0, 42.0, pVehicle) == true);
    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    // Note: 70 is not below 42 + 20, the authored altitude is preserved
    QVERIFY2(qAbs(pVehicle->geoloc().Altitude - 70.0) < 0.01, qPrintable(QString("Unexpected preserved altitude %1").arg(pVehicle->geoloc().Altitude)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Resting joystick axes report exactly zero.
*/
/***************************************************************************/
void CTestScene::testJoystickDeadZoneReportsZeroAtRest()
{
    QVERIFY(CStandardController::applyJoystickDeadZone(0.0) == 0.0);
    QVERIFY(CStandardController::applyJoystickDeadZone(0.1) == 0.0);
    QVERIFY(CStandardController::applyJoystickDeadZone(-0.14) == 0.0);
    QVERIFY(CStandardController::applyJoystickDeadZone(0.5) == 0.5);
    QVERIFY(CStandardController::applyJoystickDeadZone(-1.0) == -1.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief An attached heavy vehicle settles without launching.
*/
/***************************************************************************/
void CTestScene::testAttachedVehicleSettlesWithoutLaunch()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle = createHeavyTestVehicle(CGeoloc(19.237738, 10.896701, 42.0));
    cScene.addComponent(pVehicle);

    cScene.ensurePhysicsReference(pVehicle->geoloc());

    QVERIFY(registerFlatTestGrid(cScene, pVehicle->geoloc(), 42.0) >= 0);

    cScene.ensureVehiclePhysics();

    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    double dStartAltitude = pVehicle->geoloc().Altitude;
    Math::CVector3 vStartECEF = pVehicle->geoloc().toVector3();

    // Note: measure from the lifted physics spawn, not the arcade pose
    cScene.updateScene(1.0 / 60.0);
    double dSpawnAltitude = pVehicle->geoloc().Altitude;

    for (int iStep = 0; iStep < 179; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);
    }

    // Note: the spawn lift settles into suspension sag, never a launch
    double dAltitude = pVehicle->geoloc().Altitude;

    QVERIFY2(dStartAltitude > 41.9 && dStartAltitude < 42.1, qPrintable(QString("Unexpected arcade altitude %1").arg(dStartAltitude)));
    QVERIFY2(dSpawnAltitude > dStartAltitude && dSpawnAltitude < dStartAltitude + 1.0, qPrintable(QString("Unexpected spawn altitude %1").arg(dSpawnAltitude)));
    QVERIFY2(dAltitude > dSpawnAltitude - 1.0 && dAltitude < dSpawnAltitude + 0.25, qPrintable(QString("Unexpected altitude %1").arg(dAltitude)));

    // Note: gravity points down the reference, a settled spawn does not slide
    Math::CVector3 vTravelledM = pVehicle->geoloc().toVector3() - vStartECEF;
    Math::CVector3 vReferenceUp = pVehicle->geoloc().getTopocentricAxis().Up;
    Math::CVector3 vHorizontalM = vTravelledM - vReferenceUp * vTravelledM.dot(vReferenceUp);

    QVERIFY2(vHorizontalM.magnitude() < 1.0, qPrintable(QString("Unexpected slide %1").arg(vHorizontalM.magnitude())));

    // Note: the chassis never tips over while settling
    Math::CQuaternion cRotation;
    QVERIFY(cScene.physicsWorld()->vehicleRotation(pVehicle->physicsVehicleIndex(), cRotation) == true);

    Math::CVector3 vUp = cRotation * Math::CVector3(0.0, 1.0, 0.0);

    QVERIFY2(vUp.dot(vReferenceUp) > 0.9, qPrintable(QString("Unexpected up %1 %2 %3").arg(vUp.X).arg(vUp.Y).arg(vUp.Z)));

    // Note: every tire found the ground
    for (int iWheel = 0; iWheel < 4; iWheel++)
    {
        QVERIFY2(cScene.physicsWorld()->vehicleWheelHasContact(pVehicle->physicsVehicleIndex(), iWheel) == true, qPrintable(QString("Wheel %1 without contact").arg(iWheel)));
    }

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A driven heavy vehicle stays grounded without flipping.
*/
/***************************************************************************/
void CTestScene::testDrivenVehicleStaysGrounded()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle = createHeavyTestVehicle(CGeoloc(19.237738, 10.896701, 42.0));
    cScene.addComponent(pVehicle);

    cScene.ensurePhysicsReference(pVehicle->geoloc());

    QVERIFY(registerFlatTestGrid(cScene, pVehicle->geoloc(), 42.0) >= 0);

    cScene.ensureVehiclePhysics();

    QVERIFY(pVehicle->hasPhysicsVehicle() == true);

    Math::CVector3 vStartECEF = pVehicle->geoloc().toVector3();
    Math::CVector3 vReferenceUp = pVehicle->geoloc().getTopocentricAxis().Up;

    pVehicle->setDriverInputNorm(1.0, 0.0, 0.0, 0.0);

    double dMinimumUp = 1.0;
    double dMaximumAltitude = pVehicle->geoloc().Altitude;

    for (int iStep = 0; iStep < 300; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);

        Math::CQuaternion cRotation;
        cScene.physicsWorld()->vehicleRotation(pVehicle->physicsVehicleIndex(), cRotation);

        double dUp = (cRotation * Math::CVector3(0.0, 1.0, 0.0)).dot(vReferenceUp);

        if (dUp < dMinimumUp)
        {
            dMinimumUp = dUp;
        }

        if (pVehicle->geoloc().Altitude > dMaximumAltitude)
        {
            dMaximumAltitude = pVehicle->geoloc().Altitude;
        }
    }

    // Note: full throttle moves the vehicle forward on its wheels
    double dTravelledM = (pVehicle->geoloc().toVector3() - vStartECEF).magnitude();

    QVERIFY2(dTravelledM > 3.0, qPrintable(QString("Unexpected travel %1").arg(dTravelledM)));

    // Note: no takeoff and no flip, the chassis up never leaves the top half
    QVERIFY2(dMaximumAltitude < 45.0, qPrintable(QString("Unexpected altitude %1").arg(dMaximumAltitude)));
    QVERIFY2(dMinimumUp > 0.5, qPrintable(QString("Unexpected minimum up %1").arg(dMinimumUp)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

/***************************************************************************/
/*!
* \brief Wires a PVP like vehicle to the scene like the application does. \br\br
* Adds the vehicle, freezes the reference, registers a flat grid, selects the
* vehicle controller into the scene and attaches the physics vehicle. Link
* targets default to the parent vehicle with the trajectory left enabled but
* empty, like CController::solveLinks does. Returns false on failure.
*/
/***************************************************************************/
static bool setupPvpLikeScene(C3DScene& cScene, QSP<CVehicle>& pVehicle, QSP<CForceController>& pController)
{
    pVehicle = createPvpLikeVehicle(CGeoloc(19.2380, 10.8960, 42.0));
    cScene.addComponent(pVehicle);

    cScene.ensurePhysicsReference(pVehicle->geoloc());

    if (registerFlatTestGrid(cScene, pVehicle->geoloc(), 42.0) < 0)
    {
        return false;
    }

    // Note: the application selects the vehicle controller into the scene on
    // combo selection
    foreach (QSP<CComponent> pChild, pVehicle->childComponents())
    {
        QSP<CForceController> pCandidate = QSP_CAST(CForceController, pChild);

        if (pCandidate != nullptr)
        {
            pController = pCandidate;
            break;
        }
    }

    if (pController == nullptr)
    {
        return false;
    }

    pController->setPositionTarget(pVehicle);
    pController->setRotationTarget(pVehicle);
    pVehicle->setTrajectoryEnabled(true);

    cScene.setController(pController.data());

    if (cScene.controller() != pController.data())
    {
        return false;
    }

    cScene.ensureVehiclePhysics();

    return pVehicle->hasPhysicsVehicle() == true;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Presses H then Z on the scene controller, like CView::keyPressEvent. \br\br
* H releases the hand brake, Z holds full throttle with no release.
*/
/***************************************************************************/
static void pressHandBrakeAndForwardKeys(C3DScene& cScene)
{
    QKeyEvent cBrakePress(QEvent::KeyPress, Qt::Key_H, Qt::NoModifier);
    cScene.controller()->keyPressEvent(&cBrakePress);

    QKeyEvent cForwardPress(QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier);
    cScene.controller()->keyPressEvent(&cForwardPress);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A PVP like vehicle driven through scene controller key events moves.
*/
/***************************************************************************/
void CTestScene::testKeyDrivenPvpLikeVehicleMoves()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle;
    QSP<CForceController> pController;

    QVERIFY(setupPvpLikeScene(cScene, pVehicle, pController) == true);
    QVERIFY(pVehicle->isHandBrakeEngaged() == true);

    pressHandBrakeAndForwardKeys(cScene);

    QVERIFY(pVehicle->isHandBrakeEngaged() == false);

    Math::CVector3 vStartECEF = pVehicle->geoloc().toVector3();

    for (int iStep = 0; iStep < 300; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);
    }

    double dTravelledM = (pVehicle->geoloc().toVector3() - vStartECEF).magnitude();

    QVERIFY2(dTravelledM > 3.0, qPrintable(QString("Key driven PVP covered %1 m").arg(dTravelledM)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A PVP like vehicle parked with the hand brake moves when H then Z arrive.
*/
/***************************************************************************/
void CTestScene::testKeyDrivenPvpLikeVehicleMovesAfterIdle()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle;
    QSP<CForceController> pController;

    QVERIFY(setupPvpLikeScene(cScene, pVehicle, pController) == true);

    // Note: the application parks vehicles with the hand brake engaged long
    // before the user drives, idle past the Jolt sleep delay with no keys
    for (int iStep = 0; iStep < 120; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);
    }

    cScene.physicsWorld()->logVehicleDebugState(pVehicle->physicsVehicleIndex());

    pressHandBrakeAndForwardKeys(cScene);

    QVERIFY(pVehicle->isHandBrakeEngaged() == false);

    Math::CVector3 vStartECEF = pVehicle->geoloc().toVector3();

    for (int iStep = 0; iStep < 300; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);

        if (iStep == 60)
        {
            cScene.physicsWorld()->logVehicleDebugState(pVehicle->physicsVehicleIndex());
        }
    }

    cScene.physicsWorld()->logVehicleDebugState(pVehicle->physicsVehicleIndex());

    double dTravelledM = (pVehicle->geoloc().toVector3() - vStartECEF).magnitude();

    CVehicleDebugState cState;
    cScene.physicsWorld()->vehicleDebugState(pVehicle->physicsVehicleIndex(), cState);

    QVERIFY2(dTravelledM > 3.0, qPrintable(QString("Idle PVP covered %1 m active=%2 speed=%3 fwd=%4 hand=%5 rpm=%6 gear=%7")
        .arg(dTravelledM)
        .arg(cState.m_bBodyActive ? 1 : 0)
        .arg(cState.m_dBodySpeedMS, 0, 'f', 3)
        .arg(cState.m_dForwardReadback, 0, 'f', 2)
        .arg(cState.m_dHandBrakeReadback, 0, 'f', 2)
        .arg(cState.m_dEngineRPM, 0, 'f', 1)
        .arg(cState.m_iGear)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A PVP like vehicle holding Z and C curves to the right of its start heading.
*/
/***************************************************************************/
void CTestScene::testKeyDrivenPvpLikeVehicleTurnsRight()
{
    C3DScene cScene(false);

    QSP<CVehicle> pVehicle;
    QSP<CForceController> pController;

    QVERIFY(setupPvpLikeScene(cScene, pVehicle, pController) == true);

    pressHandBrakeAndForwardKeys(cScene);

    QKeyEvent cRightPress(QEvent::KeyPress, Qt::Key_C, Qt::NoModifier);
    cScene.controller()->keyPressEvent(&cRightPress);

    Math::CVector3 vStartECEF = pVehicle->geoloc().toVector3();
    Math::CAxis aStartAxis = pVehicle->geoloc().getTopocentricAxis();

    for (int iStep = 0; iStep < 120; iStep++)
    {
        cScene.updateScene(1.0 / 60.0);
    }

    cScene.physicsWorld()->logVehicleDebugState(pVehicle->physicsVehicleIndex());

    Math::CVector3 vTravelledM = pVehicle->geoloc().toVector3() - vStartECEF;
    double dForwardM = vTravelledM.dot(aStartAxis.Front);
    double dRightM = vTravelledM.dot(aStartAxis.Right);

    CVehicleDebugState cState;
    cScene.physicsWorld()->vehicleDebugState(pVehicle->physicsVehicleIndex(), cState);

    double dFrontSteerRad = cState.m_vWheelSteerAngleRad.count() > 0 ? cState.m_vWheelSteerAngleRad[0] : 0.0;

    QVERIFY2(dForwardM > 2.0 && dRightM > 1.0, qPrintable(QString("Forward %1 m lateral %2 m right steer=%3 rad")
        .arg(dForwardM).arg(dRightM).arg(dFrontSteerRad)));

    cScene.detachVehicleFromPhysics(pVehicle);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A fresh scene carries the default rendering environment values.
*/
/***************************************************************************/
void CTestScene::testEnvironmentDefaultsMatchConstants()
{
    C3DScene cScene(false);

    QCOMPARE(cScene.atmosphereAltitude(), DEFAULT_ATMOSPHERE_ALTITUDE);
    QCOMPARE(cScene.fogMinimumDistance(), DEFAULT_FOG_MINIMUM_DISTANCE);
    QCOMPARE(cScene.underwaterFogDistance(), DEFAULT_UNDERWATER_FOG_DISTANCE);

    QCOMPARE(cScene.globalAmbientColor().X, DEFAULT_GLOBAL_AMBIENT_R);
    QCOMPARE(cScene.globalAmbientColor().Y, DEFAULT_GLOBAL_AMBIENT_G);
    QCOMPARE(cScene.globalAmbientColor().Z, DEFAULT_GLOBAL_AMBIENT_B);

    QCOMPARE(cScene.fogColor().X, DEFAULT_FOG_COLOR_R);
    QCOMPARE(cScene.fogColor().Y, DEFAULT_FOG_COLOR_G);
    QCOMPARE(cScene.fogColor().Z, DEFAULT_FOG_COLOR_B);

    QCOMPARE(cScene.skyColor().X, DEFAULT_SKY_COLOR_R);
    QCOMPARE(cScene.skyColor().Y, DEFAULT_SKY_COLOR_G);
    QCOMPARE(cScene.skyColor().Z, DEFAULT_SKY_COLOR_B);

    QCOMPARE(cScene.ozoneAmount(), DEFAULT_OZONE_AMOUNT);

    QVERIFY2(cScene.skyModel() != nullptr, "Scene should own a sky model");
    QCOMPARE(cScene.skyModelName(), QString(SKY_MODEL_PHYSICAL));
    QCOMPARE(cScene.skyModelId(), SKY_MODEL_PHYSICAL_ID);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief An Environment node overrides the rendering environment values.
*/
/***************************************************************************/
void CTestScene::testEnvironmentLoadsFromXmlNode()
{
    C3DScene cScene(false);

    QString sXml =
        "<Scene>"
        "    <Environment AtmosphereAltitude=\"200000.0\" FogMinimumDistance=\"25.0\" UnderwaterFogDistance=\"500.0\" SkyModel=\"Physical\" OzoneAmount=\"1.5\">"
        "        <GlobalAmbient r=\"0.1\" g=\"0.2\" b=\"0.3\" />"
        "        <SkyColor r=\"0.4\" g=\"0.5\" b=\"0.6\" />"
        "        <FogColor r=\"0.7\" g=\"0.8\" b=\"0.9\" />"
        "    </Environment>"
        "</Scene>";

    CXMLNode tDoc = CXMLNode::parseXML(sXml);
    QVERIFY2(tDoc.getNodeByTagName("Environment").isEmpty() == false, "Environment node not parsed");

    cScene.loadEnvironment(tDoc.getNodeByTagName("Environment"));

    QCOMPARE(cScene.atmosphereAltitude(), 200000.0);
    QCOMPARE(cScene.fogMinimumDistance(), 25.0);
    QCOMPARE(cScene.underwaterFogDistance(), 500.0);
    QCOMPARE(cScene.skyModelName(), QString("Physical"));
    QCOMPARE(cScene.skyModelId(), SKY_MODEL_PHYSICAL_ID);
    QCOMPARE(cScene.ozoneAmount(), 1.5);

    QCOMPARE(cScene.globalAmbientColor().X, 0.1);
    QCOMPARE(cScene.globalAmbientColor().Y, 0.2);
    QCOMPARE(cScene.globalAmbientColor().Z, 0.3);

    QCOMPARE(cScene.fogColor().X, 0.7);
    QCOMPARE(cScene.fogColor().Y, 0.8);
    QCOMPARE(cScene.fogColor().Z, 0.9);

    QCOMPARE(cScene.skyColor().X, 0.4);
    QCOMPARE(cScene.skyColor().Y, 0.5);
    QCOMPARE(cScene.skyColor().Z, 0.6);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief An unknown sky model name keeps the current model active.
*/
/***************************************************************************/
void CTestScene::testUnknownSkyModelKeepsCurrentModel()
{
    C3DScene cScene(false);

    cScene.setSkyModel("Bogus");

    QVERIFY2(cScene.skyModel() != nullptr, "Scene should still own a sky model");
    QCOMPARE(cScene.skyModelName(), QString(SKY_MODEL_PHYSICAL));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The sky model factory builds the physical model and rejects the rest.
*/
/***************************************************************************/
void CTestScene::testSkyModelFactoryBuildsPhysicalModel()
{
    CSkyModel* pPhysical = CSkyModel::create("Physical");

    QVERIFY2(pPhysical != nullptr, "Factory should build the physical model");
    QCOMPARE(pPhysical->name(), QString(SKY_MODEL_PHYSICAL));
    QCOMPARE(pPhysical->id(), SKY_MODEL_PHYSICAL_ID);

    delete pPhysical;

    QVERIFY2(CSkyModel::create("Bogus") == nullptr, "Factory should reject unknown models");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A high sun renders warm white, ordered red over green over blue.
*/
/***************************************************************************/
void CTestScene::testSunlightIsWarmWhiteAtNoon()
{
    Math::CVector3 vNoon = computeSunlightColor(1.0);

    QVERIFY2(vNoon.X > 1.0 && vNoon.Y > 1.0 && vNoon.Z > 1.0, "Noon sun should stay bright");
    QVERIFY2(vNoon.X > vNoon.Y && vNoon.Y > vNoon.Z, "Noon sun should stay warm white");
    QVERIFY2(vNoon.Z / vNoon.X > 0.75, "Noon sun should not be red");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A sun near the horizon renders orange red, blue mostly scattered away.
*/
/***************************************************************************/
void CTestScene::testSunlightTurnsRedAtHorizon()
{
    Math::CVector3 vHorizon = computeSunlightColor(0.02);

    QVERIFY2(vHorizon.X > vHorizon.Y && vHorizon.Y > vHorizon.Z, "Horizon sun should lose blue first");
    QVERIFY2(vHorizon.Z < 0.05, "Horizon sun should have almost no blue left");
    QVERIFY2(vHorizon.X < 1.0, "Horizon sun should be dimmer than noon");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A sun below the horizon renders black.
*/
/***************************************************************************/
void CTestScene::testSunlightIsBlackAtNight()
{
    Math::CVector3 vNight = computeSunlightColor(-0.2);

    QCOMPARE(vNight.X, 0.0);
    QCOMPARE(vNight.Y, 0.0);
    QCOMPARE(vNight.Z, 0.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The horizon sky under a high sun has no green cast with ozone.
*
* Mirrors the shader scattering equation: sun transmittance times scattering
* share times view absorption. Without ozone the product peaks in green,
* with the nominal ozone amount red dominates again. The sweep covers grazing
* to high sun so the dusk and dawn cases stay red, never green.
*/
/***************************************************************************/
void CTestScene::testHorizonSkyHasNoGreenCastWithOzone()
{
    double dViewPath = DEFAULT_RAYLEIGH_SCALE_HEIGHT * CAtmosphereOptics::computeAirmass(0.0);

    double dSinElevations[] = { 0.02, 0.05, 0.1, 0.2, 0.5 };

    for (unsigned int iIndex = 0; iIndex < sizeof(dSinElevations) / sizeof(dSinElevations[0]); iIndex++)
    {
        double dSinSun = dSinElevations[iIndex];

        Math::CVector3 vSun = CAtmosphereOptics::computeSunTransmittance(
            dSinSun,
            DEFAULT_RAYLEIGH_SCALE_HEIGHT,
            DEFAULT_RAYLEIGH_BETA_R,
            DEFAULT_RAYLEIGH_BETA_G,
            DEFAULT_RAYLEIGH_BETA_B,
            DEFAULT_OZONE_BETA_R,
            DEFAULT_OZONE_BETA_G,
            DEFAULT_OZONE_BETA_B,
            DEFAULT_OZONE_AMOUNT);

        Math::CVector3 vShare = CAtmosphereOptics::computeScatteringShare(
            DEFAULT_RAYLEIGH_BETA_R,
            DEFAULT_RAYLEIGH_BETA_G,
            DEFAULT_RAYLEIGH_BETA_B,
            DEFAULT_OZONE_BETA_R,
            DEFAULT_OZONE_BETA_G,
            DEFAULT_OZONE_BETA_B,
            DEFAULT_OZONE_AMOUNT);

        Math::CVector3 vExtinction = CAtmosphereOptics::computeExtinction(
            DEFAULT_RAYLEIGH_BETA_R,
            DEFAULT_RAYLEIGH_BETA_G,
            DEFAULT_RAYLEIGH_BETA_B,
            DEFAULT_OZONE_BETA_R,
            DEFAULT_OZONE_BETA_G,
            DEFAULT_OZONE_BETA_B,
            DEFAULT_OZONE_AMOUNT);

        Math::CVector3 vSky(
            vSun.X * vShare.X * (1.0 - exp(-vExtinction.X * dViewPath)),
            vSun.Y * vShare.Y * (1.0 - exp(-vExtinction.Y * dViewPath)),
            vSun.Z * vShare.Z * (1.0 - exp(-vExtinction.Z * dViewPath)));

        QVERIFY2(vSky.X >= vSky.Y, "Ozone horizon sky should not peak in green");

        if (dSinSun <= 0.05)
        {
            QVERIFY2(vSky.X > 2.0 * vSky.Y, "Grazing sun horizon should be strongly red");
            QVERIFY2(vSky.Z <= vSky.Y, "Grazing sun horizon should have almost no blue left");
        }
    }

    Math::CVector3 vSunBare = CAtmosphereOptics::computeSunTransmittance(
        0.5,
        DEFAULT_RAYLEIGH_SCALE_HEIGHT,
        DEFAULT_RAYLEIGH_BETA_R,
        DEFAULT_RAYLEIGH_BETA_G,
        DEFAULT_RAYLEIGH_BETA_B,
        DEFAULT_OZONE_BETA_R,
        DEFAULT_OZONE_BETA_G,
        DEFAULT_OZONE_BETA_B,
        0.0);

    double dBareViewR = 1.0 - exp(-DEFAULT_RAYLEIGH_BETA_R * dViewPath);
    double dBareViewG = 1.0 - exp(-DEFAULT_RAYLEIGH_BETA_G * dViewPath);

    QVERIFY2(vSunBare.Y * dBareViewG > vSunBare.X * dBareViewR, "Bare Rayleigh sky should peak in green");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Setting the ozone amount clamps negative values and keeps the tint valid.
*/
/***************************************************************************/
void CTestScene::testOzoneAmountClampsNegativeValues()
{
    C3DScene cScene(false);

    cScene.setOzoneAmount(-2.0);

    QCOMPARE(cScene.ozoneAmount(), 0.0);

    Math::CVector3 vNoon = computeSunlightColor(1.0, cScene.ozoneAmount());

    QVERIFY2(vNoon.X > vNoon.Y && vNoon.Y > vNoon.Z, "Ozone free noon sun should stay warm white");
}
