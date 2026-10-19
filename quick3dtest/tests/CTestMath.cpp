
// Quick3D
#include "CVector2.h"
#include "CVector3.h"
#include "CMatrix4.h"

// Application
#include "CTestMath.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestMath::CTestMath()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Center point maps to zero angles.
*/
/***************************************************************************/
void CTestMath::testCenterMapsToZeroAngles()
{
    CVector2 vViewportSize(1000, 1000);
    CVector2 vAngles = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.5, 0.5));

    QVERIFY(qFuzzyIsNull(vAngles.X));
    QVERIFY(qFuzzyIsNull(vAngles.Y));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Left and right points give opposite horizontal angles.
*/
/***************************************************************************/
void CTestMath::testHorizontalSymmetry()
{
    CVector2 vViewportSize(1000, 1000);

    CVector2 vLeft = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.25, 0.5));
    CVector2 vRight = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.75, 0.5));

    QVERIFY(vLeft.X < 0.0);
    QVERIFY(vRight.X > 0.0);
    QVERIFY(qFuzzyCompare(1.0 + vLeft.X, 1.0 - vRight.X));
    QVERIFY(qFuzzyIsNull(vLeft.Y));
    QVERIFY(qFuzzyIsNull(vRight.Y));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Bottom and top points give opposite vertical angles.
*/
/***************************************************************************/
void CTestMath::testVerticalSymmetry()
{
    CVector2 vViewportSize(1000, 1000);

    CVector2 vBottom = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.5, 0.0));
    CVector2 vTop = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.5, 1.0));

    QVERIFY(vBottom.Y < 0.0);
    QVERIFY(vTop.Y > 0.0);
    QVERIFY(qFuzzyCompare(1.0 + vBottom.Y, 1.0 - vTop.Y));
    QVERIFY(qFuzzyIsNull(vBottom.X));
    QVERIFY(qFuzzyIsNull(vTop.X));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Angles grow with the distance from the center.
*/
/***************************************************************************/
void CTestMath::testAnglesGrowFromCenter()
{
    CVector2 vViewportSize(1000, 1000);

    CVector2 vNear = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.6, 0.5));
    CVector2 vFar = CVector2::pointToAngles(vViewportSize, 90.0, CVector2(0.9, 0.5));

    QVERIFY(vFar.X > vNear.X);
    QVERIFY(vNear.X > 0.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Direction built from angles is a unit vector.
*/
/***************************************************************************/
void CTestMath::testDirectionIsUnitVector()
{
    CVector2 vViewportSize(1000, 1000);
    CMatrix4 tTransform;

    const double dSamples[][2] = { { 0.0, 0.5 }, { 0.25, 0.5 }, { 1.0, 0.0 }, { 1.0, 1.0 } };

    for (unsigned int iIndex = 0; iIndex < sizeof(dSamples) / sizeof(dSamples[0]); iIndex++)
    {
        CVector2 vPoint(dSamples[iIndex][0], dSamples[iIndex][1]);
        CVector2 vAngles = CVector2::pointToAngles(vViewportSize, 90.0, vPoint);

        CVector3 vAngles3(vAngles.Y, vAngles.X, 0.0);

        CRay3 tRay;
        tRay.vOrigin = tTransform * CVector3();
        tRay.vNormal = CMatrix4::makeRotation(vAngles3) * CVector3(0.0, 0.0, 1.0);
        tRay.vNormal = (tTransform * tRay.vNormal) - tRay.vOrigin;

        QVERIFY(qFuzzyCompare(1.0 + tRay.vNormal.magnitude(), 2.0));
    }
}

//-------------------------------------------------------------------------------------------------
