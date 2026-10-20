
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestVehicle : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestVehicle();

private slots:

    //! Wheel defaults match the physics wheel defaults
    void testWheelDefaultsMatchPhysicsDefaults();

    //! Wheel parameters load from the General XML node
    void testWheelLoadsParametersFromXML();

    //! A vehicle builds one physics wheel per wheel child
    void testVehicleBuildsPhysicsWheelsFromChildren();

    //! A vehicle without wheels refuses physics attachment
    void testVehicleWithoutWheelsRefusesAttachment();

    //! Key presses report normalized drive inputs
    void testControllerReportsNormalizedDriveInput();

    //! A force controller drives an attached vehicle through driver inputs
    void testWheeledControllerDrivesAttachedVehicle();

    //! The hand brake is engaged by default and toggles with the H key
    void testHandBrakeEngagedByDefault();

    //! The hand brake flag loads from XML and reaches the physics wheels
    void testHandBrakeReachesPhysicsWheels();

    //! A released heavy vehicle drives, an engaged hand brake holds it back
    void testHeavyVehicleDrivesOnlyWhenReleased();

    //! All wheels of a settled heavy vehicle report ground contact
    void testHeavyVehicleWheelsTouchGround();
};

//-------------------------------------------------------------------------------------------------
