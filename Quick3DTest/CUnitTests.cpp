
// Qt
#include <QDebug>

// Quick3D
#include "CComponentFactory.h"

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
    QVector<CVector2> vNormalizedPoints;
    CVector2 viewportSize(1024, 768);
    CMatrix4 transform;
    CRay3 ray;
    double dFOV = 90.0;

    vNormalizedPoints << CVector2(0.00, 0.5);
    vNormalizedPoints << CVector2(0.25, 0.5);
    vNormalizedPoints << CVector2(0.50, 0.5);
    vNormalizedPoints << CVector2(0.75, 0.5);
    vNormalizedPoints << CVector2(1.00, 0.5);

    vNormalizedPoints << CVector2( 0.00, 0.0);
    vNormalizedPoints << CVector2( 0.50, 0.0);
    vNormalizedPoints << CVector2( 1.00, 0.0);

    foreach (CVector2 vPoint, vNormalizedPoints)
    {
        CVector2 vAngles2 = CVector2::pointToAngles(viewportSize, dFOV, vPoint);

        CVector3 vAngles3(vAngles2.Y, vAngles2.X, 0.0);

        ray.vOrigin = transform * CVector3();
        ray.vNormal = CMatrix4::makeRotation(vAngles3) * CVector3(0.0, 0.0, 1.0);
        ray.vNormal = (transform * ray.vNormal) - ray.vOrigin;

        qDebug() << "Point =" << vPoint.X << "," << vPoint.Y;
        qDebug() << "Angles = " << Angles::toDeg(vAngles2.X) << "," << Angles::toDeg(vAngles2.Y);
        qDebug() << "Ray =" << ray.vNormal.X << "," << ray.vNormal.Y << "," << ray.vNormal.Z;
    }
}
