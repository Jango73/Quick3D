
#pragma once

#include "quick3d_global.h"

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "CQ3DConstants.h"
#include "CStandardController.h"
#include "CComponentReference.h"
#include "CWing.h"
#include "CElevator.h"
#include "CRudder.h"
#include "CEngine.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CAircraftController : public CStandardController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CAircraftController(C3DScene* pScene);

    //!
    virtual ~CAircraftController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CAircraftController; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //!
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    //!
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    //!
    virtual void q3dEvent(CQ3DEvent* event) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool                            m_bAileronLeft;
    bool                            m_bAileronRight;
    bool                            m_bNoseUp;
    bool                            m_bNoseDown;
    bool                            m_bRudderLeft;
    bool                            m_bRudderRight;
    bool                            m_bEngine1ThrustUp;
    bool                            m_bEngine2ThrustUp;
    bool                            m_bEngine1ThrustDown;
    bool                            m_bEngine2ThrustDown;

    CComponentReference<CWing>      m_rLeftWingTarget;
    CComponentReference<CWing>      m_rRightWingTarget;
    CComponentReference<CElevator>  m_rElevatorTarget;
    CComponentReference<CRudder>    m_rRudderTarget;
    CComponentReference<CEngine>    m_rEngine1Target;
    CComponentReference<CEngine>    m_rEngine2Target;
    CComponentReference<CEngine>    m_rEngine3Target;
    CComponentReference<CEngine>    m_rEngine4Target;
};
