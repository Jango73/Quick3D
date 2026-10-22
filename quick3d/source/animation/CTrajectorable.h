
#pragma once

// qt-plus
#include "CLogger.h"
#include "CAverager.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CMesh.h"
#include "CTrajectory.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTrajectorable : public CMesh
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CTrajectorable(C3DScene* pScene);

    //!
    virtual ~CTrajectorable();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setSpeedMS(double value) { m_dSpeedMS = value; }

    //!
    void setTurnSpeedDS(double value) { m_dTurnSpeedDS = value; }

    //!
    void setTrajectoryEnabled(bool value) { m_bTrajectoryEnabled = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    double getSpeedMS() const { return m_dSpeedMS; }

    //!
    double getTurnSpeedDS() const { return m_dTurnSpeedDS; }

    //!
    CTrajectory& getTrajectory() { return m_tTrajectory; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CTrajectorable; }

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual bool isTrajectorable() const { return true; }

    //! Returns a JSON document with the movement state (geoloc from the base, rotation, speeds)
    virtual QString snapshotJson() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void resetTrajectory();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    //! Returns the shared rotation and speed fields used by snapshotJson() overrides
    QString snapshotMovementJson() const;

    bool                        m_bTrajectoryEnabled;
    double                      m_dSpeedMS;
    double                      m_dTurnSpeedDS;
    CTrajectory                 m_tTrajectory;
    CAverager<Math::CVector3>   m_aRotation;
};
