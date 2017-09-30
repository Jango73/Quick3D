
// qt-plus
#include "CLogger.h"

// Application
#include "CAxis.h"
#include "C3DScene.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CVehicle.h"
#include "CMesh.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CVehicle::instantiator(C3DScene* pScene)
{
    return new CVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CVehicle::CVehicle(C3DScene* pScene)
    : CTrajectorable(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CVehicle::~CVehicle()
{
}

//-------------------------------------------------------------------------------------------------

void CVehicle::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CTrajectorable::loadParameters(sBaseFile, xComponent);

    // Read physics

    CXMLNode tPhysicsNode = xComponent.getNodeByTagName(ParamName_Physics);

    setSpeedMS(tPhysicsNode.attributes()["SpeedMS"].toDouble());
    setTurnSpeedDS(tPhysicsNode.attributes()["TurnSpeedDS"].toDouble());

    // Read trajectory

    CXMLNode tTrajectoryNode = xComponent.getNodeByTagName("Trajectory");
    QVector<CXMLNode> vPointNodes = tTrajectoryNode.getNodesByTagName(ParamName_Geoloc);

    foreach (CXMLNode tPointNode, vPointNodes)
    {
        m_tTrajectory.addPoint(CGeoloc(
                                   tPointNode.attributes()[ParamName_Latitude].toDouble(),
                                   tPointNode.attributes()[ParamName_Longitude].toDouble(),
                                   tPointNode.attributes()[ParamName_Altitude].toDouble()
                                   ));
    }

    CXMLNode tGeolocNode = xComponent.getNodeByTagName(ParamName_Geoloc);
    CXMLNode tRotationNode = xComponent.getNodeByTagName("Rotation");

    CGeoloc gPosition = CGeoloc(
                tGeolocNode.attributes()[ParamName_Latitude].toDouble(),
                tGeolocNode.attributes()[ParamName_Longitude].toDouble(),
                tGeolocNode.attributes()[ParamName_Altitude].toDouble()
                );

    CVector3 vRotation = CVector3(
                Math::Angles::toRad(tRotationNode.attributes()[ParamName_x].toDouble()),
                Math::Angles::toRad(tRotationNode.attributes()[ParamName_y].toDouble()),
                Math::Angles::toRad(tRotationNode.attributes()[ParamName_z].toDouble())
                );

    m_tTrajectory.setPosition(gPosition);
    m_tTrajectory.setRotation(vRotation);
}

//-------------------------------------------------------------------------------------------------

void CVehicle::update(double dDeltaTime)
{
    CTrajectorable::update(dDeltaTime);

    if (isRootObject() && m_bPhysicsActive)
    {
        if (m_pFields.count() > 0.0)
        {
            CVector3 vRotation = rotation();
            CBoundingBox box = bounds();

            CMatrix4 mOrientation = CMatrix4().makeRotation(vRotation);

            CGeoloc gBackLeft = CGeoloc(geoloc(), mOrientation * CVector3(box.minimum().X * 0.8, 0.0, box.minimum().Z * 0.8));
            CGeoloc gBackRight = CGeoloc(geoloc(), mOrientation * CVector3(box.maximum().X * 0.8, 0.0, box.minimum().Z * 0.8));
            CGeoloc gFrontLeft = CGeoloc(geoloc(), mOrientation * CVector3(box.minimum().X * 0.8, 0.0, box.maximum().Z * 0.8));
            CGeoloc gFrontRight = CGeoloc(geoloc(), mOrientation * CVector3(box.maximum().X * 0.8, 0.0, box.maximum().Z * 0.8));

            double r1 = 0.0;
            double r2 = 0.0;
            double r3 = 0.0;
            double r4 = 0.0;

            double hBackLeft = m_pFields[0]->getHeightAt(gBackLeft, &r1);
            double hBackRight = m_pFields[0]->getHeightAt(gBackRight, &r2);
            double hFrontLeft = m_pFields[0]->getHeightAt(gFrontLeft, &r3);
            double hFrontRight = m_pFields[0]->getHeightAt(gFrontRight, &r4);

            // double dRigidness = (r1 + r2 + r3 + r4) / 4.0;

            double hAverage = (hBackLeft + hBackRight + hFrontLeft + hFrontRight) / 4.0;

            if (hBackLeft != Q3D_INFINITY && hBackRight != Q3D_INFINITY && hFrontLeft != Q3D_INFINITY && hFrontRight != Q3D_INFINITY)
            {
                if (fabs(geoloc().Altitude - hAverage) < 5.0)
                {
                    double hdBackLeft = gBackLeft.Altitude - hBackLeft;
                    double hdBackRight = gBackRight.Altitude - hBackRight;
                    double hdFrontLeft = gFrontLeft.Altitude - hFrontLeft;
                    double hdFrontRight = gFrontRight.Altitude - hFrontRight;

                    // X
                    CVector3 Front = CVector3(0.0, (hdFrontLeft + hdFrontRight) / 2.0, box.maximum().Z * 0.8);
                    CVector3 Back = CVector3(0.0, (hdBackLeft + hdBackRight) / 2.0, box.minimum().Z * 0.8);

                    double dDiffX = (Front - Back).eulerXAngle();

                    // Z
                    CVector3 Right = CVector3(box.maximum().X * 0.8, (hdBackRight + hdFrontRight) / 2.0, 0.0);
                    CVector3 Left = CVector3(box.minimum().X * 0.8, (hdBackLeft + hdFrontLeft) / 2.0, 0.0);

                    double dDiffZ = (Right - Left).eulerZAngle();

                    CVector3 torque(-dDiffX * 4.0, 0.0, -dDiffZ);

                    addLocalTorque_kg(torque * totalMass_kg() * 0.5);
                }
            }

//            CVector3 vRotation = rotation();
//            CMatrix4 mOrientation = CMatrix4().makeRotation(vRotation);
//            QVector<CContactPoint> vPoints = contactPoints();

//            foreach (CContactPoint point, vPoints)
//            {
//                CGeoloc gPosition = CGeoloc(geoloc(), mOrientation * point.position());
//                double dRigidness = 0.0;
//                double dHeight = m_pFields[0]->getHeightAt(gPosition, &dRigidness);

//                CVector3 vSeg = gPosition.toVector3();

//                m_pScene->addSegment(vSeg, vSeg + CVector3(0.5, 0.0, 0.0));
//                m_pScene->addSegment(vSeg, vSeg + CVector3(0.0, 0.5, 0.0));
//                m_pScene->addSegment(vSeg, vSeg + CVector3(0.0, 0.0, 0.5));

//                if (dHeight != Q3D_INFINITY)
//                {
//                    double dDiff = gPosition.Altitude - dHeight;

//                    if (dDiff > 0.0 && dDiff < 5000.0)
//                    {
//                        double dForce_kg = dDiff * totalMass_kg() * -0.001;
//                        addUncenteredLocalForce_kg(point.position(), dForce_kg);
//                    }
//                }
//            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CVehicle::paint(CRenderContext* pContext)
{
    CTrajectorable::paint(pContext);
}

//-------------------------------------------------------------------------------------------------

QVector<CContactPoint> CVehicle::contactPoints()
{
    return CTrajectorable::contactPoints();
}
