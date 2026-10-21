
// Quick3D
#include "C3DScene.h"
#include "CCamera.h"
#include "CLight.h"
#include "CViewport.h"

// Application
#include "CTestCamera.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestCamera::CTestCamera()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Builds a scene with a camera at the origin, for ray tests.
* \return The viewport carrying the camera.
* \note The scene is intentionally not deleted: C3DScene teardown outside
* \note the render loop is not covered, tests only need the scene alive.
*/
/***************************************************************************/
static CViewport* buildViewportAtOrigin(C3DScene*& pScene, QSP<CCamera>& pCamera)
{
    pScene = new C3DScene();
    pCamera = QSP<CCamera>(new CCamera(pScene));

    CViewport* pViewport = new CViewport(pScene);
    pViewport->setSize(CVector2(1024, 768));
    pViewport->setCamera(pCamera);

    pScene->viewports()[0] = pViewport;

    pCamera->setGeoloc(CGeoloc(0.0, 0.0, 0.0));
    pCamera->computeWorldTransform();

    return pViewport;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Every screen point produces a unit direction ray from the camera position.
*/
/***************************************************************************/
void CTestCamera::testRaysAreUnitVectorsFromCamera()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    CViewport* pViewport = buildViewportAtOrigin(pScene, pCamera);

    const double dSamples[][2] = { { 0, 0 }, { 500, 0 }, { 1000, 0 }, { 0, 500 }, { 500, 500 }, { 1000, 500 }, { 0, 1000 }, { 500, 1000 }, { 1000, 1000 } };

    CRay3 tFirstRay = pCamera->screenPointToWorldRay(pViewport, CVector2(dSamples[0][0], dSamples[0][1]));

    for (unsigned int iIndex = 0; iIndex < sizeof(dSamples) / sizeof(dSamples[0]); iIndex++)
    {
        CRay3 tRay = pCamera->screenPointToWorldRay(pViewport, CVector2(dSamples[iIndex][0], dSamples[iIndex][1]));

        // Direction is normalized
        QVERIFY(qFuzzyCompare(1.0 + tRay.vNormal.magnitude(), 2.0));

        // Every ray starts at the camera position
        QVERIFY(qFuzzyCompare(1.0 + tRay.vOrigin.X, 1.0 + tFirstRay.vOrigin.X));
        QVERIFY(qFuzzyCompare(1.0 + tRay.vOrigin.Y, 1.0 + tFirstRay.vOrigin.Y));
        QVERIFY(qFuzzyCompare(1.0 + tRay.vOrigin.Z, 1.0 + tFirstRay.vOrigin.Z));
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Same screen point always produces the same ray.
*/
/***************************************************************************/
void CTestCamera::testRaysAreDeterministic()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    CViewport* pViewport = buildViewportAtOrigin(pScene, pCamera);

    CRay3 tFirst = pCamera->screenPointToWorldRay(pViewport, CVector2(750, 250));
    CRay3 tSecond = pCamera->screenPointToWorldRay(pViewport, CVector2(750, 250));

    QCOMPARE(tSecond.vOrigin.X, tFirst.vOrigin.X);
    QCOMPARE(tSecond.vOrigin.Y, tFirst.vOrigin.Y);
    QCOMPARE(tSecond.vOrigin.Z, tFirst.vOrigin.Z);
    QCOMPARE(tSecond.vNormal.X, tFirst.vNormal.X);
    QCOMPARE(tSecond.vNormal.Y, tFirst.vNormal.Y);
    QCOMPARE(tSecond.vNormal.Z, tFirst.vNormal.Z);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Distinct screen points produce distinct rays.
*/
/***************************************************************************/
void CTestCamera::testDistinctPointsGiveDistinctRays()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    CViewport* pViewport = buildViewportAtOrigin(pScene, pCamera);

    CRay3 tLeft = pCamera->screenPointToWorldRay(pViewport, CVector2(0, 500));
    CRay3 tRight = pCamera->screenPointToWorldRay(pViewport, CVector2(1000, 500));
    CRay3 tCenter = pCamera->screenPointToWorldRay(pViewport, CVector2(500, 500));

    QVERIFY(tLeft.vNormal.X != tRight.vNormal.X || tLeft.vNormal.Y != tRight.vNormal.Y || tLeft.vNormal.Z != tRight.vNormal.Z);
    QVERIFY(tLeft.vNormal.X != tCenter.vNormal.X || tLeft.vNormal.Y != tCenter.vNormal.Y || tLeft.vNormal.Z != tCenter.vNormal.Z);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Camera pitch stays within +-89 degrees, other axes and lights untouched.
*/
/***************************************************************************/
void CTestCamera::testPitchIsLimited()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    buildViewportAtOrigin(pScene, pCamera);

    const double dMaxPitch = Angles::toRad(CCamera::MaxPitchDegrees);

    // Pitch above limit is capped, yaw and roll are preserved
    pCamera->setRotation(CVector3(Angles::toRad(100.0), 0.5, 0.25));
    QCOMPARE(pCamera->rotation().X, dMaxPitch);
    QCOMPARE(pCamera->rotation().Y, 0.5);
    QCOMPARE(pCamera->rotation().Z, 0.25);

    // Pitch below limit is capped
    pCamera->setRotation(CVector3(Angles::toRad(-100.0), 0.0, 0.0));
    QCOMPARE(pCamera->rotation().X, -dMaxPitch);

    // Pitch within limits is preserved
    pCamera->setRotation(CVector3(Angles::toRad(45.0), 0.0, 0.0));
    QCOMPARE(pCamera->rotation().X, Angles::toRad(45.0));

    // Lights bypass the limit since shadow lights aim downwards
    QSP<CCamera> pLight(new CLight(pScene));
    pLight->setRotation(CVector3(Angles::toRad(100.0), 0.0, 0.0));
    QCOMPARE(pLight->rotation().X, Angles::toRad(100.0));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief limit caps free-look pitch used by controllers, other axes pass through.
*/
/***************************************************************************/
void CTestCamera::testLimitCapsViewPitch()
{
    const double dMaxPitch = Angles::toRad(CCamera::MaxPitchDegrees);

    // Free-look increments beyond the limit are capped
    QCOMPARE(CCamera::limit(CVector3(Angles::toRad(100.0), 0.5, 0.25)).X, dMaxPitch);
    QCOMPARE(CCamera::limit(CVector3(Angles::toRad(-100.0), 0.0, 0.0)).X, -dMaxPitch);

    // Angles within limits pass through untouched, other axes preserved
    CVector3 vFree = CCamera::limit(CVector3(Angles::toRad(45.0), 0.5, 0.25));
    QCOMPARE(vFree.X, Angles::toRad(45.0));
    QCOMPARE(vFree.Y, 0.5);
    QCOMPARE(vFree.Z, 0.25);
}

//-------------------------------------------------------------------------------------------------
