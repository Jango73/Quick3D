
#pragma once

// qt-plus
#include "CAverager.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CTrajectorable.h"
#include "CMeshInstance.h"
#include "CPhysicsWorld.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;
class CWheel;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVehicle : public CTrajectorable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CVehicle(C3DScene* pScene);

    //! Destructor
    virtual ~CVehicle();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CVehicle; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void paint(CRenderContext* pContext) Q_DECL_OVERRIDE;

    //!
    virtual QVector<CContactPoint> contactPoints() Q_DECL_OVERRIDE;

    //! Returns true when the vehicle is registered in a physics world
    virtual bool hasPhysicsBody() const Q_DECL_OVERRIDE;

    //! Returns the movement state plus physics registration, wheels and contacts as a node, JSON through CXMLNode
    virtual CXMLNode snapshotNode() Q_DECL_OVERRIDE;

    //! Returns true, vehicles accept normalized driver inputs
    virtual bool isDriveable() const Q_DECL_OVERRIDE { return true; }

    //! Forwards normalized driver inputs to the physics vehicle, does nothing when detached
    virtual void setDriverInputNorm(double dForwardNorm, double dRightNorm, double dBrakeNorm, double dHandBrakeNorm) Q_DECL_OVERRIDE;

    //! Engages or releases the hand brake, engaged by default so vehicles hold slopes at spawn
    virtual void setHandBrakeEngaged(bool value) Q_DECL_OVERRIDE { m_bHandBrakeEngaged = value; }

    //! Returns true when the hand brake is engaged
    virtual bool isHandBrakeEngaged() const Q_DECL_OVERRIDE { return m_bHandBrakeEngaged; }

    //! Returns the authored speed limit in meters per second
    virtual double driveSpeedLimitMS() const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Returns the wheel children of this vehicle, whatever the vehicle class
    QVector<QSP<CWheel> > wheels() const;

    //! Returns true when this vehicle carries at least one wheel
    bool hasWheels();

    //! Returns the chassis half extents from the first mesh child bounds, zeros without bounds
    Math::CVector3 chassisHalfExtentsM() const;

    //! Returns the chassis center offset from the first mesh child bounds, zeros without bounds
    Math::CVector3 chassisCenterOffsetM() const;

    //! Builds one physics wheel per CWheel child, in child order
    QVector<CVehicleWheel> buildPhysicsWheels() const;

    //! Registers the chassis and its wheels in the given world, returns false without wheels or on failure. \br\br
    //! \a vLocalPositionM is the vehicle origin, the body spawns shifted by \a vCenterOffsetM rotated into the chassis frame.
    bool attachPhysicsVehicle(CPhysicsWorld* pWorld, const Math::CVector3& vChassisHalfExtentsM, const Math::CVector3& vLocalPositionM, const Math::CQuaternion& cRotation = Math::CQuaternion(0.0, 0.0, 0.0, 1.0), const Math::CVector3& vCenterOffsetM = Math::CVector3(0.0, 0.0, 0.0));

    //! Removes the vehicle from its physics world, if any
    void detachPhysicsVehicle();

    //! Returns true when the vehicle is registered in a physics world
    bool hasPhysicsVehicle() const { return m_pPhysicsWorld != nullptr && m_iPhysicsVehicle >= 0; }

    //! Returns the physics world index of the vehicle, -1 when detached
    int physicsVehicleIndex() const { return m_iPhysicsVehicle; }

    //! Returns the registered physics world, nullptr when detached. \br\br
    //! Observing pointer only, the scene owns the world and detaches every
    //! registered vehicle before clearing it, never a second ownership chain.
    CPhysicsWorld* physicsWorld() const { return m_pPhysicsWorld; }

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Returns the bounds of the first direct child mesh, an empty box without mesh child
    CBoundingBox firstMeshBounds() const;

    //! Resolves the first mesh bounds, false without usable bounds
    bool validChassisBounds(CBoundingBox& cBounds) const;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QVector<CMeshInstance*>     m_vAxisMeshes;
    CPhysicsWorld*              m_pPhysicsWorld; ///< Registered world, observing only, set by attach and cleared by detach
    int                         m_iPhysicsVehicle;
    bool                        m_bHandBrakeEngaged;
};
