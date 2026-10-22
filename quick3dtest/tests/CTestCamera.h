
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestCamera : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestCamera();

private slots:

    //! Every screen point produces a unit direction ray from the camera position
    void testRaysAreUnitVectorsFromCamera();

    //! Same screen point always produces the same ray
    void testRaysAreDeterministic();

    //! Distinct screen points produce distinct rays
    void testDistinctPointsGiveDistinctRays();

    //! Camera pitch is limited to +-89 degrees, lights are not limited
    void testPitchIsLimited();

    //! limit helper caps free-look pitch, preserves other axes
    void testLimitCapsViewPitch();

    //! A sphere spanning the near plane is kept, near ground must survive the close pass
    void testFrustumKeepsSphereSpanningNearPlane();

    //! A sphere fully behind the camera is rejected
    void testFrustumRejectsSphereBehindCamera();

    //! Every point the GPU projection draws is kept by the frustum test, at any aspect
    void testFrustumKeepsEveryProjectedPoint();
};

//-------------------------------------------------------------------------------------------------
