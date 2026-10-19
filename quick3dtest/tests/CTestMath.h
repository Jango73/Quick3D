
#pragma once

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

class CTestMath : public CTestFixture
{
    Q_OBJECT

public:

    explicit CTestMath();

private slots:

    //! Center point maps to zero angles
    void testCenterMapsToZeroAngles();

    //! Left and right points give opposite horizontal angles
    void testHorizontalSymmetry();

    //! Bottom and top points give opposite vertical angles
    void testVerticalSymmetry();

    //! Angles grow with the distance from the center
    void testAnglesGrowFromCenter();

    //! Direction built from angles is a unit vector
    void testDirectionIsUnitVector();
};

//-------------------------------------------------------------------------------------------------
