
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CComponent;
class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CContactPoint
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CContactPoint()
        : m_dRadius(0.0)
    {
    }

    //! Constructor with parameters
    CContactPoint(Math::CVector3 vPosition, double dRadius = 0.0)
        : m_vPosition(vPosition)
        , m_dRadius(dRadius)
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the position
    Math::CVector3 position() const { return m_vPosition; }

    //! Returns the radius
    double radius() const { return m_dRadius; }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3  m_vPosition;
    double          m_dRadius;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPhysicalComponent : public CComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Enums
    //-------------------------------------------------------------------------------------------------

    enum ECollisionType
    {
        ctSphere,
        ctBox,
        ctMesh
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CPhysicalComponent(C3DScene* pScene);

    //! Destructor
    virtual ~CPhysicalComponent() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the drag
    void setDrag_norm(double value) { m_dDrag_norm = value; }

    //! Sets the angular drag
    void setAngularDrag_norm(double value) { m_dAngularDrag_norm = value; }

    //! Sets the mass in kilograms
    void setMass_kg(double value) { m_dMass_kg = value; }

    //! Sets the velocity in meters per second
    void setVelocity_ms(Math::CVector3 value) { m_vVelocity_ms = value; }

    //! Sets the angular velocity in radians per second
    void setAngularVelocity_rs(Math::CVector3 value) { m_vAngularVelocity_rs = value; }

    //! Sets the acceleration in meters per second per second
    void setAcceleration_mss(Math::CVector3 value) { m_vSummedForces_mss = value; }

    //! Sets the angular acceleration in meters per second per second
    void setAngularAcceleration_rss(Math::CVector3 value) { m_vSummedTorques_rss = value; }

    //! If bEnabled is true, collisions for the component will be computed.
    void setCollisions(bool bEnabled);

    //! Sets the type of collision to use
    void setCollisionType (ECollisionType eType);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CPhysicalComponent; }

    //! Return the drag
    double drag_norm() const { return m_dDrag_norm; }

    //! Returns the angular drag
    double angularDrag_norm() const { return m_dAngularDrag_norm; }

    //! Returns the mass in kilograms
    double mass_kg() const { return m_dMass_kg; }

    //! Returns the total mass in kilograms (children included)
    double totalMass_kg() const;

    //! Returns the velocity vector in meters per second
    Math::CVector3 velocity_ms() const { return m_vVelocity_ms; }

    //! Returns the angular velocity in radians per second
    Math::CVector3 angularVelocity_rs() const { return m_vAngularVelocity_rs; }

    //! Returns sum of all forces in meters per second per second
    Math::CVector3 summedForces_mss()  const { return m_vSummedForces_mss; }

    //! Returns sum of all torques in radians per second per second
    Math::CVector3 summedTorques_rss() const { return m_vSummedTorques_rss; }

    //! Returns \c true if collisions are active
    bool collisionsActive() const { return m_bCollisionsActive; }

    //! Returns the hull type used for collisions
    ECollisionType collisionType() const { return m_eCollisionType; }

    //!
    Math::CVector3 velocityVectorAngles() const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Assign operator
    CPhysicalComponent& operator = (const CPhysicalComponent& target);

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Calls the update method for child components
    virtual void postUpdate(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //! Returns a list of contact points for this component
    virtual QVector<CContactPoint> contactPoints();

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //! Adds a height field to this component
    void addField(CHeightField* value);

    //! Removes this component from physical computations.
    void sleep();

    //! Adds this component to physical computations.
    void wakeUp();

    //! Adds a local force to the component, relative to a position. It can also generate torque.
    void addUncenteredLocalForce_kg(Math::CVector3 vPosition, Math::CVector3 vForce_kg);

    //! Adds a local force to the component.
    void addLocalForce_kg(Math::CVector3 vForce_kg);

    //! Adds a force to the component, expressed in the geocentric frame.
    void addForce_kg(Math::CVector3 vForce_kg);

    //! Adds a local torque to the component.
    void addLocalTorque_kg(Math::CVector3 vTorque_kg);

    //! Adds a torque to the component, expressed in the geocentric frame.
    void addTorque_kg(Math::CVector3 vTorque_kg);

    //-------------------------------------------------------------------------------------------------
    // Static methods
    //-------------------------------------------------------------------------------------------------

    //!
    static void computeCollisions(QVector<QSP<CComponent> >& vComponents, double dDeltaTimeS);

    //!
    static void computeCollisionsForComponent(QSP<CPhysicalComponent> pPhysical, QVector<QSP<CComponent> >& vOtherComponents, double dDeltaTimeS);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool                    m_bPhysicsActive;               // Physics computations are active
    bool                    m_bCollisionsActive;            // Collision computations are active
    bool                    m_bOnGround;                    // Is object on ground?
    double                  m_dDrag_norm;                   // Normalized drag factor (0..1)
    double                  m_dAngularDrag_norm;            // Normalized angular drag factor (0..1)
    double                  m_dFriction_norm;               // Normalized friction factor (0..1)
    double                  m_dMass_kg;                     // Mass in kilograms
    double                  m_dStickToTopocentric;          //
    double                  m_dRotationLatency;             //
    Math::CVector3          m_vCenterOfMass;                // Center of mass
    Math::CVector3          m_vVelocity_ms;                 // Velocity in meters per second, topocentric frame
    Math::CVector3          m_vAngularVelocity_rs;          // Angular velocity in radians per second, topocentric frame
    Math::CVector3          m_vSummedForces_mss;            // Acceleration in meters per second per second, topocentric frame
    Math::CVector3          m_vSummedTorques_rss;           // Angular acceleration in meters per second per second, topocentric frame
    ECollisionType          m_eCollisionType;               // Type of collision hull
};
