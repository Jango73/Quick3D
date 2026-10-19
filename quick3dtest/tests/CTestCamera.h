
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
};

//-------------------------------------------------------------------------------------------------
