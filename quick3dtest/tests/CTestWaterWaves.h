
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestWaterWaves : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestWaterWaves();

private slots:

    //! Calm water has zero height and an up normal everywhere
    void testCalmWaterIsFlat();

    //! Wave height scales linearly with the wind level
    void testWindScalesHeightLinearly();

    //! Wave height changes over time at a fixed position
    void testTimeMovesWaves();

    //! The buoyancy plane normal is unit length and points upward
    void testSurfaceNormalIsUnitAndUp();

    //! The material query matches the canonical definition with scene wind and time
    void testMaterialDelegatesToCanonicalDefinition();

    //! Both vertex shaders carry the canonical wave constants
    void testShaderMirrorsCanonicalConstants();

    //! A fixed sample locks the Perlin implementation against silent drift
    void testHeightRegressionAnchor();
};

//-------------------------------------------------------------------------------------------------
