
// Application
#include "Angles.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CPhysicalComponent.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CPhysicalComponent
    \brief The base class for all 3D entities that have computed physical properties, like velocity and collisions.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CPhysicalComponent.
*/
CComponent* CPhysicalComponent::instantiator(C3DScene* pScene)
{
    return new CPhysicalComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CPhysicalComponent with its default parameters.
*/
CPhysicalComponent::CPhysicalComponent(C3DScene* pScene)
    : CComponent(pScene)
    , m_bPhysicsActive(true)
    , m_bCollisionsActive(true)
    , m_bOnGround(false)
    , m_dDrag_norm(0.1)
    , m_dAngularDrag_norm(0.1)
    , m_dFriction_norm(0.1)
    , m_dMass_kg(1.0)
    , m_dStickToNOLL(0.0)
    , m_dRotationLatency(0.0)
    , m_eCollisionType(ctSphere)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CPhysicalComponent.
*/
CPhysicalComponent::~CPhysicalComponent()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Copies properties of \a target to this component.
*/
CPhysicalComponent& CPhysicalComponent::operator = (const CPhysicalComponent& target)
{
    CComponent::operator = (target);

    m_bPhysicsActive                = target.m_bPhysicsActive;
    m_bCollisionsActive             = target.m_bCollisionsActive;
    m_dDrag_norm                    = target.m_dDrag_norm;
    m_dAngularDrag_norm             = target.m_dAngularDrag_norm;
    m_dMass_kg                      = target.m_dMass_kg;
    m_dStickToNOLL                  = target.m_dStickToNOLL;
    m_dRotationLatency              = target.m_dRotationLatency;
    m_vVelocity_ms                  = target.m_vVelocity_ms;
    m_vAngularVelocity_rs           = target.m_vAngularVelocity_rs;
    m_vSummedForces_mss             = target.m_vSummedForces_mss;
    m_vSummedTorques_rss            = target.m_vSummedTorques_rss;
    m_eCollisionType                = target.m_eCollisionType;

    return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent.
*/
void CPhysicalComponent::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);

    // Physical properties

    CXMLNode xPhysicsNode = xComponent.getNodeByTagName(ParamName_Physics);
    CXMLNode xCenterOfMassNode = xComponent.getNodeByTagName(ParamName_CenterOfMass);

    if (xPhysicsNode.isEmpty() == false)
    {
        if (xPhysicsNode.attributes()["MassKG"].isEmpty() == false)
        {
            m_dMass_kg = xPhysicsNode.attributes()["MassKG"].toDouble();
        }

        if (xPhysicsNode.attributes()["Drag"].isEmpty() == false)
        {
            m_dDrag_norm = xPhysicsNode.attributes()["Drag"].toDouble();
        }

        if (xPhysicsNode.attributes()["AngularDrag"].isEmpty() == false)
        {
            m_dAngularDrag_norm = xPhysicsNode.attributes()["AngularDrag"].toDouble();
        }

        if (xPhysicsNode.attributes()["Friction"].isEmpty() == false)
        {
            m_dFriction_norm = xPhysicsNode.attributes()["Friction"].toDouble();
        }

        if (xPhysicsNode.attributes()["StickToNOLL"].isEmpty() == false)
        {
            m_dStickToNOLL = xPhysicsNode.attributes()["StickToNOLL"].toDouble();
        }

        if (xPhysicsNode.attributes()["RotationLatency"].isEmpty() == false)
        {
            m_dRotationLatency = xPhysicsNode.attributes()["RotationLatency"].toDouble();
        }

        if (xPhysicsNode.attributes()[ParamName_PhysicsActive].isEmpty() == false)
        {
            m_bPhysicsActive = (bool) xPhysicsNode.attributes()[ParamName_PhysicsActive].toInt();
        }

        if (xPhysicsNode.attributes()[ParamName_CollisionsActive].isEmpty() == false)
        {
            m_bCollisionsActive = (bool) xPhysicsNode.attributes()[ParamName_CollisionsActive].toInt();
        }
    }

    if (xCenterOfMassNode.isEmpty() == false)
    {
        m_vCenterOfMass = CVector3(
                    xCenterOfMassNode.attributes()[ParamName_x].toDouble(),
                    xCenterOfMassNode.attributes()[ParamName_y].toDouble(),
                    xCenterOfMassNode.attributes()[ParamName_z].toDouble()
                    );
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes this object from physical computations.
*/
void CPhysicalComponent::sleep()
{
    m_bPhysicsActive = false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds this object to physical computations.
*/
void CPhysicalComponent::wakeUp()
{
    m_bPhysicsActive = true;
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bEnabled is \c true, collisions for the component will be computed.
*/
void CPhysicalComponent::setCollisions(bool bEnabled)
{
    m_bCollisionsActive = bEnabled;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the total mass (including children) of the component.
*/
double CPhysicalComponent::totalMass_kg() const
{
    double dTotalMass_kg = m_dMass_kg;

    foreach (const QSP<CComponent> pChild, m_vChildren)
    {
        const QSP<CPhysicalComponent> pPhysical = QSP_CAST(CPhysicalComponent, pChild);

        if (pPhysical != nullptr)
        {
            dTotalMass_kg += pPhysical->totalMass_kg();
        }
    }

    return dTotalMass_kg;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the euler angles of the velocity vector.
*/
CVector3 CPhysicalComponent::velocityVectorAngles() const
{
    return eulerAngles(m_vVelocity_ms);
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local force to the component, relative to a position. It can also generate torque. \br\br
    \a vForce_kg is the force to apply. \br
    \a vPosition is the local position at which the force is applied.
*/
void CPhysicalComponent::addUncenteredLocalForce_kg(CVector3 vPosition, CVector3 vForce_kg)
{
    if (m_bPhysicsActive == true)
    {
        CVector3 vSavedForce_kg = vForce_kg;

        double dDistanceAttenuation = (1.0 / (1.0 + vPosition.magnitude())) * 2.3;
        vForce_kg *= dDistanceAttenuation;
        addLocalForce_kg(vForce_kg);

        double dDistanceMultiplier = vPosition.magnitude() * 0.001;

        CVector3 vForceNormalized = vSavedForce_kg.normalized();

        CAxis aPositionAxis(eulerAngles(vPosition));

        double dXForce = aPositionAxis.Up.dot(vForceNormalized);
        double dYForce = aPositionAxis.Right.dot(vForceNormalized);

        CVector3 vForceOnAxis(dXForce, dYForce, 0.0);

        CAxis aForceAxis;

        aForceAxis = aForceAxis.transferTo(aPositionAxis);
        aForceAxis = aForceAxis.rotate(vForceOnAxis);
        aForceAxis = aForceAxis.transferFrom(aPositionAxis);

        addLocalTorque_kg(aForceAxis.eulerAngles() * (vSavedForce_kg.magnitude() * dDistanceMultiplier));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local force to the component. \br\br
    \a vForce_kg is the force to apply.
*/
void CPhysicalComponent::addLocalForce_kg(CVector3 vForce_kg)
{
    if (m_bPhysicsActive == true)
    {
        CVector3 vRotation = rotation();

        vForce_kg = CMatrix4().makeRotation(CVector3(0.0, 0.0, vRotation.Z)) * vForce_kg;
        vForce_kg = CMatrix4().makeRotation(CVector3(vRotation.X, 0.0, 0.0)) * vForce_kg;
        vForce_kg = CMatrix4().makeRotation(CVector3(0.0, vRotation.Y, 0.0)) * vForce_kg;

        m_vSummedForces_mss = m_vSummedForces_mss + (vForce_kg / totalMass_kg()) * 5.0;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a force to the component, expressed in the geocentric frame. \br\br
    \a vForce_kg is the force to apply.
*/
void CPhysicalComponent::addForce_kg(CVector3 vForce_kg)
{
    if (m_bPhysicsActive == true)
    {
        m_vSummedForces_mss = m_vSummedForces_mss + (vForce_kg / totalMass_kg()) * 5.0;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local torque to the component. \br\br
    \a vForce_kg is the torque to apply.
*/
void CPhysicalComponent::addLocalTorque_kg(CVector3 vTorque_kg)
{
    if (m_bPhysicsActive == true)
    {
        m_vSummedTorques_rss = m_vSummedTorques_rss + (vTorque_kg / totalMass_kg()) * 5.0;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a torque to the component, expressed in the geocentric frame. \br\br
    \a vForce_kg is the torque to apply.
*/
void CPhysicalComponent::addTorque_kg(CVector3 vTorque_kg)
{
    if (m_bPhysicsActive == true)
    {
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame. \br\br
    If the component is not asleep, all physics are computed here, except collisions.
*/
void CPhysicalComponent::update(double dDeltaTimeS)
{
    CComponent::update(dDeltaTimeS);

    if (isRootObject())
    {
        if (m_pFields.count() > 0)
        {
            if (dDeltaTimeS > 0.0)
            {
                CVector3 vNewPosition = position();
                CVector3 vNewRotation = rotation();

                if (m_bPhysicsActive == true)
                {
                    double dTotalMass_kg = totalMass_kg();
                    // double dAirForceFactor = CAtmosphere::getInstance()->getAirForceFactor(geoloc().Altitude);
                    double dAirDragFactor = CAtmosphere::getInstance()->airDragFactor(geoloc().Altitude);

                    // Add gravity force

                    CVector3 vGravityForce = CVector3(0.0, -dTotalMass_kg * 2.0, 0.0);

                    vGravityForce = CMatrix4().makeInverseRotation(vNewRotation * -1.0) * vGravityForce;

                    addUncenteredLocalForce_kg(m_vCenterOfMass, vGravityForce);

                    // Add drag

                    double dVelocitySquared_ms = m_vVelocity_ms.magnitude();

                    dVelocitySquared_ms = dVelocitySquared_ms * dVelocitySquared_ms;

                    double dDrag = dVelocitySquared_ms * m_dDrag_norm * dAirDragFactor * dTotalMass_kg;

                    CVector3 vDragForce = m_vVelocity_ms.normalized() * -dDrag;

                    addForce_kg(vDragForce);

                    // Update axial velocity according to force accumulators

                    m_vVelocity_ms = m_vVelocity_ms + (m_vSummedForces_mss * dDeltaTimeS);

                    // Update angular velocity according to torque accumulators

                    m_vAngularVelocity_rs = m_vAngularVelocity_rs + (m_vSummedTorques_rss * dDeltaTimeS);

                    // Add angular drag

                    m_vAngularVelocity_rs = m_vAngularVelocity_rs - (m_vAngularVelocity_rs * ((m_dAngularDrag_norm * dDeltaTimeS) * dAirDragFactor));

                    if (m_bOnGround)
                    {
                        m_vVelocity_ms = m_vVelocity_ms - (m_vVelocity_ms * (m_dFriction_norm * dDeltaTimeS));
                        m_vAngularVelocity_rs = m_vAngularVelocity_rs - (m_vAngularVelocity_rs * (m_dFriction_norm * dDeltaTimeS));
                    }

                    // Get the NOLL reference (North-Oriented Local Level)

                    CAxis aLocalAxis(geoloc().getNOLLAxis());

                    // Rotate the body around its local axis

                    CAxis aRotationAxis(vNewRotation);
                    CAxis aVelocityAxis(vNewRotation);
                    aVelocityAxis = aVelocityAxis.transferFrom(aRotationAxis);
                    aVelocityAxis = aVelocityAxis.rotate(m_vAngularVelocity_rs * dDeltaTimeS);
                    aVelocityAxis = aVelocityAxis.transferTo(aRotationAxis);
                    vNewRotation = aVelocityAxis.eulerAngles();

                    // Translate the body

                    vNewPosition += aLocalAxis.Right * m_vVelocity_ms.X * dDeltaTimeS;
                    vNewPosition += aLocalAxis.Up * m_vVelocity_ms.Y * dDeltaTimeS;
                    vNewPosition += aLocalAxis.Front * m_vVelocity_ms.Z * dDeltaTimeS;
                }

                // Manage altitude

                CGeoloc gNewGeoloc(vNewPosition);

                double dBoundsYOffset = bounds().minimum().Y;
                double dLowestAltitude = gNewGeoloc.Altitude + dBoundsYOffset;

                m_bOnGround = false;

                foreach (CHeightField* pField, m_pFields)
                {
                    double dHeight = pField->getHeightAt(gNewGeoloc);

                    // Is there ground below the body?
                    if (!(fabs(dHeight - Q3D_INFINITY) < 0.01))
                    {
                        // Is the body close to the ground?
                        if (fabs(dLowestAltitude - dHeight) < 0.2)
                        {
                            m_bOnGround = true;
                        }

                        // Is the body going under the ground?
                        if (dLowestAltitude < dHeight)
                        {
                            gNewGeoloc.Altitude = dHeight - dBoundsYOffset;

                            // Reset vertical speed
                            m_vVelocity_ms.Y = 0.0;

                            m_bOnGround = true;
                        }
                        else
                        {
                            // Here we are underwater, make the body go up slowly
                            // TODO : obey to fluid laws

                            if (gNewGeoloc.Altitude < 0.0)
                            {
                                double dUpRate = 0.0 - gNewGeoloc.Altitude;
                                if (dUpRate > 3.0) dUpRate = 3.0;
                                gNewGeoloc.Altitude += dUpRate * dDeltaTimeS;
                            }
                        }
                    }
                }

                // Update the body's position if its speed is greater than 1cm/s
                // Or if it has gone below the ground

                if (m_vVelocity_ms.magnitude() > 0.01 || m_bOnGround)
                {
                    setGeoloc(gNewGeoloc);
                }

                setRotation(vNewRotation);
            }
        }
    }
    else
    {
        if (m_dStickToNOLL > 0.0)
        {
            if (m_pParent != nullptr)
            {
                if (m_pParent->previousWorldTransform().isIdentity() == false)
                {
                    CAxis anAxis(rotation());

                    anAxis = anAxis * m_pParent->previousWorldTransform();
                    anAxis = anAxis * m_pParent->worldTransformInverse();

                    setRotation(anAxis.eulerAngles());

                    // Vector3 vNewAngles = anAxis.euleurAngles();
                    // setRotation(interpolate(vNewAngles, m_vRotation, m_dStickToNOLL * dDeltaTime));
                }
            }
        }

        if (m_dRotationLatency > 0.0)
        {
            /*
            if (m_pParent != nullptr)
            {
                if (m_pParent->getPreviousWorldTransform().isIdentity() == false)
                {
                    CAxis anAxis;
                    CMatrix4 aMatrix;

                    aMatrix = aMatrix * CMatrix4::MakeRotation(rotation());
                    aMatrix = aMatrix * CMatrix4::MakeRotation(animRotation());

                    anAxis = anAxis * aMatrix;
                    anAxis = anAxis * m_pParent->getPreviousWorldTransform();
                    anAxis = anAxis * m_pParent->getWorldTransformInverse();

                    CVector3 vRotation = anAxis.euleurAngles();

                    setRotation(vRotation);
                    vRotation = getRotation();

                    vRotation.X += -vRotation.X * 2.0 * dDeltaTimeS;
                    vRotation.Y += -vRotation.Y * 2.0 * dDeltaTimeS;
                    vRotation.Z += -vRotation.Z * 2.0 * dDeltaTimeS;

                    setRotation(vRotation);
                }
            }
            */
        }
    }

    // Reset force and torque accumulators

    m_vSummedForces_mss = CVector3();
    m_vSummedTorques_rss = CVector3();

    // Show axis

    /*
    CAxis axis = geoloc().getNOLLAxis();
    CVector3 vStart = getWorldPosition();
    CVector3 vEnd = vStart + axis.Front * 2.0;
    m_pScene->addSegment(vStart, vEnd);

    vEnd = vStart + axis.Up * 2.0;
    m_pScene->addSegment(vStart, vEnd);

    vEnd = vStart + axis.Right * 2.0;
    m_pScene->addSegment(vStart, vEnd);
    */
}

//-------------------------------------------------------------------------------------------------

/*!
    Does post update work on the component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CPhysicalComponent::postUpdate(double dDeltaTimeS)
{
    CComponent::postUpdate(dDeltaTimeS);

    setPreviousWorldTransform(worldTransform());
}

//-------------------------------------------------------------------------------------------------

QVector<CContactPoint> CPhysicalComponent::contactPoints()
{
    QVector<CContactPoint> points;
    CBoundingBox box = bounds();

    points << CContactPoint(CVector3(box.minimum().X * 0.8, 0.0, box.minimum().Z * 0.8), 0.0);
    points << CContactPoint(CVector3(box.maximum().X * 0.8, 0.0, box.minimum().Z * 0.8), 0.0);
    points << CContactPoint(CVector3(box.minimum().X * 0.8, 0.0, box.maximum().Z * 0.8), 0.0);
    points << CContactPoint(CVector3(box.maximum().X * 0.8, 0.0, box.maximum().Z * 0.8), 0.0);

    return points;
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes collisions for the components in \a vComponents using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CPhysicalComponent::computeCollisions(QVector<QSP<CComponent> >& vComponents, double dDeltaTimeS)
{
    foreach (QSP<CComponent> pComponent, vComponents)
    {
        QSP<CPhysicalComponent> pPhysical = QSP_CAST(CPhysicalComponent, pComponent);

        // Terrains are for now processed in update()
        if (pPhysical && pPhysical->getClassName() != ClassName_CWorldTerrain)
        {
            if (pPhysical->isRootObject() && pPhysical->collisionsActive() == true)
            {
                // Compute collisions only if object has non null speed
                if (pPhysical->velocity_ms().magnitude() > 0.0)
                {
                    computeCollisionsForComponent(pPhysical, vComponents, dDeltaTimeS);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes collisions for \a pComponent using \a dDeltaTimeS, which is the elapsed seconds since the last frame. \br\br
    \a vOtherComponents is the list of components which it can interact with.
*/
void CPhysicalComponent::computeCollisionsForComponent(QSP<CPhysicalComponent> pPhysical, QVector<QSP<CComponent> >& vOtherComponents, double dDeltaTimeS)
{
    // Iterate through each other components
    foreach (QSP<CComponent> pOtherComponent, vOtherComponents)
    {
        QSP<CPhysicalComponent> pOtherPhysical = QSP_CAST(CPhysicalComponent, pOtherComponent);

        if (pOtherPhysical != nullptr)
        {
            if (pPhysical != pOtherPhysical && pOtherPhysical->isRootObject() && pOtherPhysical->collisionsActive() == true)
            {
                // Terrains are for now processed in update()
                if (pOtherPhysical->getClassName() != ClassName_CWorldTerrain)
                {
                    // Compute distance to other object
                    CVector3 vPosition = pPhysical->geoloc().toVector3(pOtherPhysical->geoloc());
                    double dRadiusSum = pPhysical->worldBounds().radius() + pOtherPhysical->worldBounds().radius();

                    // Do the spheres intersect?
                    if (vPosition.magnitude() < dRadiusSum)
                    {
                        // If yes, make objects bounce

                        double dForce = pPhysical->m_vVelocity_ms.magnitude() * pPhysical->totalMass_kg() * 4.0;
                        CVector3 vForceDirection = vPosition.normalized() * 0.5 * dForce;
                        double dTotalMass = pPhysical->totalMass_kg() + pOtherPhysical->totalMass_kg();
                        double dForceThisComponent = 1.0 - (pPhysical->totalMass_kg() / dTotalMass);
                        double dForceOtherComponent = 1.0 - (pOtherPhysical->totalMass_kg() / dTotalMass);
                        pPhysical->addForce_kg(vForceDirection * dForceThisComponent);
                        pOtherPhysical->addForce_kg(vForceDirection * dForceOtherComponent * -1.0);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds the the height field specified by \a value to the list of height fields that this component must take into account.
*/
void CPhysicalComponent::addField(CHeightField* value)
{
    if (m_pFields.contains(value) == false)
    {
        m_pFields.append(value);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this component to \a stream using the indentation value in \a iIdent.
*/
void CPhysicalComponent::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CPhysicalComponent]"));
    dumpIdent(stream, iIdent, QString("Physics active : %1").arg(m_bPhysicsActive));
    dumpIdent(stream, iIdent, QString("Stick to NOLL : %1").arg(m_dStickToNOLL));
    dumpIdent(stream, iIdent, QString("Drag : %1").arg(m_dDrag_norm));
    dumpIdent(stream, iIdent, QString("Angular drag : %1").arg(m_dAngularDrag_norm));
    dumpIdent(stream, iIdent, QString("Mass : %1").arg(m_dMass_kg));
    dumpIdent(stream, iIdent, QString("Velocity : %1").arg(m_vVelocity_ms.toString()));
    dumpIdent(stream, iIdent, QString("Angular velocity : %1").arg(m_vAngularVelocity_rs.toString()));

    CComponent::dump(stream, iIdent);
}
