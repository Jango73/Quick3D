
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestScene : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestScene();

private slots:

    //! Matrix to quaternion conversion inverts CMatrix4::fromQuaternion exactly
    void testQuaternionRoundTripsRotationMatrix();

    //! Converted rotations turn vectors like Jolt quaternions do
    void testConvertedRotationMatchesJolt();

    //! fromBasisVectors maps local axes to the basis in both engines
    void testBasisVectorsMapLocalAxes();

    //! Attaching a wheel less vehicle fails without freezing a reference
    void testAttachRefusesWheelLessVehicle();

    //! Attaching freezes the physics reference on the vehicle geoloc
    void testAttachFreezesPhysicsReference();

    //! Attaching aligns gravity with the frozen reference down direction
    void testAttachAlignsGravityWithReferenceDown();

    //! Attaching aligns gravity with the vehicle down when the reference is distant
    void testAttachAlignsGravityWithVehicleDown();

    //! Stepped attached vehicles fall along the reference down direction
    void testSceneStepsAttachedVehicle();

    //! A large frame delta executes at most four fixed steps
    void testFixedStepCapsPerFrame();

    //! Detaching removes the vehicle from the world
    void testDetachRemovesVehicle();

    //! A ray hits a registered height field grid at the grid altitude
    void testHeightfieldRayHitsRegisteredGrid();

    //! A ready terrain mesh copies to a north first altitude grid
    void testChunkTerrainGridCopiesReadyMesh();

    //! ensureVehiclePhysics attaches a vehicle over physics ground only
    void testEnsureAttachesVehicleOverPhysicsGround();

    //! A close spawn snaps to the measured terrain before attaching
    void testEnsureSnapsCloseSpawnToMeasuredGround();

    //! A vehicle stays detached until late terrain appears, then snaps
    void testEnsureAttachesWhenTerrainAppearsLate();

    //! A high spawn keeps its authored altitude instead of teleporting down
    void testEnsureKeepsHighSpawnAltitude();

    //! Resting joystick axes report exactly zero
    void testJoystickDeadZoneReportsZeroAtRest();

    //! An attached heavy vehicle settles without launching
    void testAttachedVehicleSettlesWithoutLaunch();

    //! A driven heavy vehicle stays grounded without flipping
    void testDrivenVehicleStaysGrounded();

    //! A PVP like vehicle driven through key events on the scene controller moves
    void testKeyDrivenPvpLikeVehicleMoves();

    //! A PVP like vehicle parked with the hand brake moves when H then Z arrive
    void testKeyDrivenPvpLikeVehicleMovesAfterIdle();

    //! A PVP like vehicle holding Z and C curves to the right of its start heading
    void testKeyDrivenPvpLikeVehicleTurnsRight();
};

//-------------------------------------------------------------------------------------------------
