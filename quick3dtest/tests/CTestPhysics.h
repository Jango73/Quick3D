
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestPhysics : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestPhysics();

private slots:

    //! Placed bodies read back at their placement through the float conversion
    void testBodyPositionRoundTripsPlacement();

    //! A dynamic sphere dropped on a static floor comes to rest on the floor
    void testSphereFallsOntoStaticFloor();

    //! Unknown body indexes report failure instead of crashing
    void testUnknownBodyReportsFailure();

    //! A chassis dropped on its wheels settles on the suspension travel
    void testVehicleSettlesOnSuspension();

    //! A driven chassis moves forward along its local +Z axis
    void testVehicleDrivesForward();

    //! A heavy driven chassis climbs a 30 percent grade from a slow start
    void testHeavyVehicleClimbsSlope();

    //! Unknown vehicle and wheel indexes report failure instead of crashing
    void testUnknownVehicleReportsFailure();
};

//-------------------------------------------------------------------------------------------------
