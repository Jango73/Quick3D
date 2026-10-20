
// qt-plus
#include "CLogger.h"

// Application
#include "CAxis.h"
#include "C3DScene.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CVehicle.h"
#include "CWheel.h"
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
    , m_pPhysicsWorld(nullptr)
    , m_iPhysicsVehicle(-1)
    , m_bHandBrakeEngaged(true)
{
}

//-------------------------------------------------------------------------------------------------

CVehicle::~CVehicle()
{
    detachPhysicsVehicle();
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

    if (isRootObject() && m_bPhysicsActive && hasPhysicsVehicle() == false)
    {
        if (m_pFields.count() > 0.0)
        {
            CVector3 vRotation = rotation();
            CBoundingBox box = bounds();

            CMatrix4 mOrientation = CMatrix4().makeRotation(vRotation);

            double dDiffX = 0.0;
            double dDiffZ = 0.0;
            bool bAlign = false;

            QVector<QSP<CWheel> > vWheels = wheels();

            if (vWheels.count() > 0)
            {
                double dFrontSum = 0.0;
                double dBackSum = 0.0;
                double dRightSum = 0.0;
                double dLeftSum = 0.0;
                int iFrontCount = 0;
                int iBackCount = 0;
                int iRightCount = 0;
                int iLeftCount = 0;
                double dGroundSum = 0.0;
                int iGroundCount = 0;
                double dLeverZ = 1.0;
                double dLeverX = 1.0;

                foreach (QSP<CWheel> pWheel, vWheels)
                {
                    if (pWheel == nullptr)
                    {
                        continue;
                    }

                    CVector3 vLocal = pWheel->position();
                    CGeoloc gWheel = CGeoloc(geoloc(), mOrientation * vLocal);

                    double dRigidness = 0.0;
                    double dHeight = m_pFields[0]->getHeightAt(gWheel, &dRigidness);

                    if (dHeight == Q3D_INFINITY)
                    {
                        continue;
                    }

                    double dClearance = (gWheel.Altitude - pWheel->radiusM()) - dHeight;

                    dGroundSum += dHeight;
                    iGroundCount++;

                    if (fabs(vLocal.Z) > dLeverZ)
                    {
                        dLeverZ = fabs(vLocal.Z);
                    }

                    if (fabs(vLocal.X) > dLeverX)
                    {
                        dLeverX = fabs(vLocal.X);
                    }

                    if (vLocal.Z > 0.01)
                    {
                        dFrontSum += dClearance;
                        iFrontCount++;
                    }
                    else if (vLocal.Z < -0.01)
                    {
                        dBackSum += dClearance;
                        iBackCount++;
                    }

                    if (vLocal.X > 0.01)
                    {
                        dRightSum += dClearance;
                        iRightCount++;
                    }
                    else if (vLocal.X < -0.01)
                    {
                        dLeftSum += dClearance;
                        iLeftCount++;
                    }
                }

                if (iGroundCount > 0)
                {
                    double hAverage = dGroundSum / double(iGroundCount);

                    if (fabs(geoloc().Altitude - hAverage) < 5.0)
                    {
                        if (iFrontCount > 0 && iBackCount > 0)
                        {
                            CVector3 Front = CVector3(0.0, dFrontSum / double(iFrontCount), dLeverZ);
                            CVector3 Back = CVector3(0.0, dBackSum / double(iBackCount), -dLeverZ);

                            dDiffX = (Front - Back).eulerXAngle();
                            bAlign = true;
                        }

                        if (iRightCount > 0 && iLeftCount > 0)
                        {
                            CVector3 Right = CVector3(dLeverX, dRightSum / double(iRightCount), 0.0);
                            CVector3 Left = CVector3(-dLeverX, dLeftSum / double(iLeftCount), 0.0);

                            dDiffZ = (Right - Left).eulerZAngle();
                            bAlign = true;
                        }
                    }
                }
            }
            else
            {
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

                        dDiffX = (Front - Back).eulerXAngle();

                        // Z
                        CVector3 Right = CVector3(box.maximum().X * 0.8, (hdBackRight + hdFrontRight) / 2.0, 0.0);
                        CVector3 Left = CVector3(box.minimum().X * 0.8, (hdBackLeft + hdFrontLeft) / 2.0, 0.0);

                        dDiffZ = (Right - Left).eulerZAngle();
                        bAlign = true;
                    }
                }
            }

            if (bAlign == true)
            {
                CVector3 torque(-dDiffX * 4.0, 0.0, -dDiffZ);

                addLocalTorque_kg(torque * totalMass_kg() * 0.5);
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

//-------------------------------------------------------------------------------------------------

/*!
    Returns the wheel children of this vehicle. \br\br
    Any vehicle class can carry wheels, direct children of class CWheel only.
*/
QVector<QSP<CWheel> > CVehicle::wheels() const
{
    QVector<QSP<CWheel> > vWheels;

    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        if (pChild == nullptr)
        {
            continue;
        }

        QSP<CWheel> pWheel = QSP_CAST(CWheel, pChild);

        if (pWheel != nullptr)
        {
            vWheels.append(pWheel);
        }
    }

    return vWheels;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when this vehicle carries at least one wheel.
*/
bool CVehicle::hasWheels()
{
    return wheels().count() > 0;
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds one physics wheel per CWheel child, in child order. \br\br
    The suspension attachment point is the wheel center: the spring travel
    below and above it maps to the suspension length range.
*/
QVector<CVehicleWheel> CVehicle::buildPhysicsWheels() const
{
    QVector<CVehicleWheel> vPhysicsWheels;

    foreach (QSP<CWheel> pWheel, wheels())
    {
        if (pWheel == nullptr)
        {
            continue;
        }

        CVehicleWheel cPhysicsWheel;
        cPhysicsWheel.m_vPosition = pWheel->position();
        cPhysicsWheel.m_dRadiusM = pWheel->radiusM();
        cPhysicsWheel.m_dWidthM = pWheel->widthM();
        cPhysicsWheel.m_dSuspensionMinLengthM = pWheel->suspensionMinLengthM();
        cPhysicsWheel.m_dSuspensionMaxLengthM = pWheel->suspensionMaxLengthM();
        cPhysicsWheel.m_dSuspensionFrequencyHz = pWheel->suspensionFrequencyHz();
        cPhysicsWheel.m_dSuspensionDampingNorm = pWheel->suspensionDampingNorm();
        cPhysicsWheel.m_dLongitudinalFrictionNorm = pWheel->longitudinalFrictionNorm();
        cPhysicsWheel.m_dLateralFrictionNorm = pWheel->lateralFrictionNorm();
        cPhysicsWheel.m_bDriven = pWheel->driven();
        cPhysicsWheel.m_bHandBrake = pWheel->handBrake();

        if (pWheel->steered() == true)
        {
            cPhysicsWheel.m_dMaxSteerAngleRad = Math::Angles::toRad(pWheel->maxSteerAngleDeg());
        }

        vPhysicsWheels.append(cPhysicsWheel);
    }

    return vPhysicsWheels;
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers the chassis and its wheels in the given world. \br\br
    The world must outlive the vehicle: call detachPhysicsVehicle() before
    destroying the world. Returns false without wheels or on failure.
*/
bool CVehicle::attachPhysicsVehicle(CPhysicsWorld* pWorld, const Math::CVector3& vChassisHalfExtentsM, const Math::CVector3& vLocalPositionM, const Math::CQuaternion& cRotation, const Math::CVector3& vCenterOffsetM)
{
    if (pWorld == nullptr || hasPhysicsVehicle() == true)
    {
        return false;
    }

    QVector<CVehicleWheel> vPhysicsWheels = buildPhysicsWheels();

    if (vPhysicsWheels.count() == 0)
    {
        LOG_METHOD_WARNING("Vehicle has no wheels");
        return false;
    }

    int iVehicle = pWorld->addVehicle(vChassisHalfExtentsM, totalMass_kg(), vLocalPositionM, vPhysicsWheels, cRotation, vCenterOffsetM);

    if (iVehicle < 0)
    {
        LOG_METHOD_WARNING("Physics world rejected the vehicle");
        return false;
    }

    m_pPhysicsWorld = pWorld;
    m_iPhysicsVehicle = iVehicle;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes the vehicle from its physics world, if any
*/
void CVehicle::detachPhysicsVehicle()
{
    if (hasPhysicsVehicle() == false)
    {
        return;
    }

    m_pPhysicsWorld->removeVehicle(m_iPhysicsVehicle);

    m_pPhysicsWorld = nullptr;
    m_iPhysicsVehicle = -1;
}

//-------------------------------------------------------------------------------------------------

/*!
    Forwards normalized driver inputs to the physics vehicle, does nothing when detached
*/
void CVehicle::setDriverInputNorm(double dForwardNorm, double dRightNorm, double dBrakeNorm, double dHandBrakeNorm)
{
    if (hasPhysicsVehicle() == false)
    {
        return;
    }

    m_pPhysicsWorld->setVehicleDriverInput(m_iPhysicsVehicle, dForwardNorm, dRightNorm, dBrakeNorm, dHandBrakeNorm);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns true when the vehicle is registered in a physics world
*/
bool CVehicle::hasPhysicsBody() const
{
    return hasPhysicsVehicle();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the chassis half extents from the first mesh child bounds, zeros without bounds. \br\br
    Reuses the mesh bounds declared in the scene file, no duplicated dimensions.
*/
Math::CVector3 CVehicle::chassisHalfExtentsM() const
{
    CBoundingBox cBounds = firstMeshBounds();

    if (cBounds.radius() <= 0.0)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    return (cBounds.maximum() - cBounds.minimum()) * 0.5;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the chassis center offset from the first mesh child bounds, zeros without bounds
*/
Math::CVector3 CVehicle::chassisCenterOffsetM() const
{
    CBoundingBox cBounds = firstMeshBounds();

    if (cBounds.radius() <= 0.0)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    return cBounds.center();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the bounds of the first direct child mesh, an empty box without mesh child
*/
CBoundingBox CVehicle::firstMeshBounds() const
{
    foreach (QSP<CComponent> pChild, m_vChildren)
    {
        if (pChild == nullptr)
        {
            continue;
        }

        CBoundingBox cBounds = pChild->bounds();

        if (cBounds.radius() > 0.0)
        {
            return cBounds;
        }
    }

    return CBoundingBox();
}
