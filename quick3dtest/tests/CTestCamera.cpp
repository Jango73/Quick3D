
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

/*!
* \brief Builds a camera-space frustum matching the render path units.
* \details computeFrustum takes radians like CCamera::render, the projection
* \details matrix takes degrees like QMatrix4x4::perspective.
*/
static void buildFrustum(CCamera& cCamera, double dFOVDegrees, double dAspectRatio, double dNear, double dFar)
{
    cCamera.computeFrustum(Angles::toRad(dFOVDegrees), dAspectRatio, dNear, dFar);
}

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

/***************************************************************************/
/*!
* \brief A sphere spanning the near plane is kept, near ground must survive the close pass.
*/
void CTestCamera::testFrustumKeepsSphereSpanningNearPlane()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    buildViewportAtOrigin(pScene, pCamera);

    buildFrustum(*pCamera, 60.0, 16.0 / 9.0, 0.1, 1000.0);

    // Sphere centered 0.5 m ahead with a 5 m radius spans the 0.1 m near plane
    QVERIFY(pCamera->contains(CVector3(0.0, 0.0, 0.5), 5.0));

    // Sphere containing the camera position is kept
    QVERIFY(pCamera->contains(CVector3(0.0, 0.0, 0.0), 100.0));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A sphere fully behind the camera is rejected.
*/
void CTestCamera::testFrustumRejectsSphereBehindCamera()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    buildViewportAtOrigin(pScene, pCamera);

    buildFrustum(*pCamera, 60.0, 16.0 / 9.0, 0.1, 1000.0);

    QVERIFY(pCamera->contains(CVector3(0.0, 0.0, -50.0), 5.0) == false);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Every point the GPU projection draws is kept by the frustum test.
* \details The projection matrix is the reference the GPU clips with, the
* \details frustum test must never reject a drawn point at any aspect ratio.
* \details Boundary samples are skipped, plane conventions may differ there
* \details without hiding any pixel.
*/
void CTestCamera::testFrustumKeepsEveryProjectedPoint()
{
    C3DScene* pScene = nullptr;
    QSP<CCamera> pCamera;
    buildViewportAtOrigin(pScene, pCamera);

    const double dFOVDegrees = 60.0;
    const double dAspects[] = { 1.0, 4.0 / 3.0, 16.0 / 9.0, 21.0 / 9.0 };

    for (unsigned int iAspect = 0; iAspect < sizeof(dAspects) / sizeof(dAspects[0]); iAspect++)
    {
        buildFrustum(*pCamera, dFOVDegrees, dAspects[iAspect], 0.1, 1000.0);

        QMatrix4x4 mProjection = CCamera::getQtProjectionMatrix(dFOVDegrees, dAspects[iAspect], 0.1, 1000.0);

        for (double dAzimuthDeg = -75.0; dAzimuthDeg <= 75.0; dAzimuthDeg += 3.0)
        {
            for (double dElevationDeg = -60.0; dElevationDeg <= 60.0; dElevationDeg += 3.0)
            {
                const double dAzimuth = Angles::toRad(dAzimuthDeg);
                const double dElevation = Angles::toRad(dElevationDeg);
                const double dDistance = 50.0;

                CVector3 vPoint(
                    dDistance * sin(dAzimuth) * cos(dElevation),
                    dDistance * sin(dElevation),
                    dDistance * cos(dAzimuth) * cos(dElevation)
                    );

                QVector4D vClip = mProjection * QVector4D(vPoint.X, vPoint.Y, vPoint.Z, 1.0);

                if (vClip.w() <= 0.0) continue;

                const double dNx = vClip.x() / vClip.w();
                const double dNy = vClip.y() / vClip.w();
                const double dNz = vClip.z() / vClip.w();

                // Skip the boundary band, plane conventions may differ there
                if (qAbs(dNx) > 0.9 || qAbs(dNy) > 0.9 || qAbs(dNz) > 0.9) continue;

                const bool bVisible = qAbs(dNx) <= 1.0 && qAbs(dNy) <= 1.0 && qAbs(dNz) <= 1.0;

                if (bVisible == false) continue;

                QVERIFY2(
                    pCamera->contains(vPoint, 0.0),
                    qPrintable(QString("Frustum rejects drawn point (%1, %2, %3) at aspect %4")
                        .arg(vPoint.X).arg(vPoint.Y).arg(vPoint.Z).arg(dAspects[iAspect])));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
