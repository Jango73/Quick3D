
// Qt
#include <QDebug>

// qt-plus
#include "CLogger.h"

// Quick3D
#include "CComponentFactory.h"
#include "CCamera.h"
#include "CGeoTree.h"
#include "CWaypoint.h"

// Application
#include "CUnitTests.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CUnitTests::CUnitTests()
{
}

//-------------------------------------------------------------------------------------------------

CUnitTests::~CUnitTests()
{
    CComponentFactory::killInstance();
}

//-------------------------------------------------------------------------------------------------

void CUnitTests::run()
{
    qDebug() << "--------------------------------------------------";
    qDebug() << "Testing CVector2::pointToAngles()";

    QVector<CVector2> vNormalizedPoints;
    QVector<CVector2> vPoints;
    CVector2 viewportSize(1000, 1000);
    CMatrix4 transform;
    double dFOV = 90.0;

    vNormalizedPoints << CVector2(0.00, 0.5);
    vNormalizedPoints << CVector2(0.25, 0.5);
    vNormalizedPoints << CVector2(0.50, 0.5);
    vNormalizedPoints << CVector2(0.75, 0.5);
    vNormalizedPoints << CVector2(1.00, 0.5);

    vNormalizedPoints << CVector2(0.00, 0.0);
    vNormalizedPoints << CVector2(0.50, 0.0);
    vNormalizedPoints << CVector2(1.00, 0.0);

    foreach (CVector2 vPoint, vNormalizedPoints)
    {
        CVector2 vAngles2 = CVector2::pointToAngles(viewportSize, dFOV, vPoint);

        CVector3 vAngles3(vAngles2.Y, vAngles2.X, 0.0);

        CRay3 ray;
        ray.vOrigin = transform * CVector3();
        ray.vNormal = CMatrix4::makeRotation(vAngles3) * CVector3(0.0, 0.0, 1.0);
        ray.vNormal = (transform * ray.vNormal) - ray.vOrigin;

        qDebug() << "--------------------";
        qDebug() << "Point =" << vPoint.X << "," << vPoint.Y;
        qDebug() << "Angles = " << Angles::toDeg(vAngles2.X) << "," << Angles::toDeg(vAngles2.Y);
        qDebug() << "Ray =" << ray.vNormal.X << "," << ray.vNormal.Y << "," << ray.vNormal.Z;
        qDebug() << "Ray length =" << ray.vNormal.magnitude();
    }

    qDebug() << "--------------------------------------------------";
    qDebug() << "Testing CCamera::screenPointToWorldRay()";

    vPoints << CVector2(0, 0);
    vPoints << CVector2(250, 0);
    vPoints << CVector2(500, 0);
    vPoints << CVector2(750, 0);
    vPoints << CVector2(1000, 0);

    vPoints << CVector2(0, 500);
    vPoints << CVector2(250, 500);
    vPoints << CVector2(500, 500);
    vPoints << CVector2(750, 500);
    vPoints << CVector2(1000, 500);

    vPoints << CVector2(0, 750);
    vPoints << CVector2(250, 750);
    vPoints << CVector2(500, 750);
    vPoints << CVector2(750, 750);
    vPoints << CVector2(1000, 750);

    vPoints << CVector2(0, 1000);
    vPoints << CVector2(250, 1000);
    vPoints << CVector2(500, 1000);
    vPoints << CVector2(750, 1000);
    vPoints << CVector2(1000, 1000);

    C3DScene* pScene = new C3DScene();
    QSP<CCamera> pCamera = QSP<CCamera>(new CCamera(pScene));

    pScene->viewports()[0] = new CViewport(pScene);
    pScene->viewports()[0]->setSize(Math::CVector2(1024, 768));
    pScene->viewports()[0]->setCamera(pCamera);

    pCamera->setGeoloc(CGeoloc(0.0, 0.0, 0.0));
    pCamera->computeWorldTransform();

    foreach (CVector2 vPoint, vPoints)
    {
        CRay3 ray = pCamera->screenPointToWorldRay(pScene->viewports()[0], vPoint);

        qDebug() << "--------------------";
        qDebug() << "Point =" << vPoint.X << "," << vPoint.Y;
        qDebug() << "Ray pos =" << ray.vOrigin.X << "," << ray.vOrigin.Y << "," << ray.vOrigin.Z;
        qDebug() << "Ray norm =" << ray.vNormal.X << "," << ray.vNormal.Y << "," << ray.vNormal.Z;
        qDebug() << "Ray length =" << ray.vNormal.magnitude();

        ray = pCamera->worldTransformInverse() * ray;
        CVector3 angles = eulerAngles(ray.vNormal);

        qDebug() << "Angles =" << Angles::toDeg(angles.X) << "," << Angles::toDeg(angles.Y) << "," << Angles::toDeg(angles.Z);
    }

    delete pScene;

    qDebug() << "--------------------------------------------------";
    qDebug() << "Testing CGeoTree";

    CWaypoint* wp1 = new CWaypoint(wtVORDME, "WP1", CGeoloc(48.999472,  2.623806, 0.0), 0.0);
    CWaypoint* wp2 = new CWaypoint(wtVORDME, "WP2", CGeoloc(45.600000,  3.400000, 0.0), 0.0);
    CWaypoint* wp3 = new CWaypoint(wtVORDME, "WP3", CGeoloc(32.500000, 20.200000, 0.0), 0.0);
    CWaypoint* wp4 = new CWaypoint(wtVORDME, "WP4", CGeoloc( 5.500000, 80.400000, 0.0), 0.0);

    CGeoTree tGeoTree(2);
    tGeoTree.append(wp1);
    tGeoTree.append(wp2);
    tGeoTree.append(wp3);
    tGeoTree.append(wp4);

    QFile dump("./Geotree.dump.txt");

    if (dump.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&dump);
        tGeoTree.dump(stream, 0);
        dump.close();
    }

    QVector<CGeolocalized*> vResult = tGeoTree.query(CGeoloc(48.5, 2.2, 0.0), 400000.0);
    qDebug() << "vResult.count() =" << vResult.count();

    delete wp1;
    delete wp2;
    delete wp3;
    delete wp4;
}
