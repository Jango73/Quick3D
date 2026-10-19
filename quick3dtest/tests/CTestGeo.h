
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestGeo : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestGeo();

private slots:

    //! Query around France finds the two French waypoints
    void testQueryFindsNearbyWaypoints();

    //! Query far from every waypoint returns nothing
    void testQueryIgnoresFarWaypoints();

    //! Dump writes a non empty file in the isolated test folder
    void testDumpCreatesFile();

    //! Geoloc built from a reference and a local vector is deterministic
    void testGeolocOffsetIsDeterministic();
};

//-------------------------------------------------------------------------------------------------
