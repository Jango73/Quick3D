
#pragma once

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CComponentReference.h"
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CQ3DEvent
{

public:

    enum EEventAction
    {
        None,
        Press,
        Release
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    CQ3DEvent()
        : m_eAction(None)
        , m_dValue(0.0)
    {
    }

    //!
    CQ3DEvent(QString sName, EEventAction eAction = Press, double dValue = 0.0)
        : m_sName(sName)
        , m_eAction(eAction)
        , m_dValue(dValue)
    {
    }

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setName(QString sName) { m_sName = sName; }

    //!
    void setAction(EEventAction eAction) { m_eAction = eAction; }

    //!
    void setValue(double dValue) { m_dValue = dValue; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString getName() const { return m_sName; }

    //!
    EEventAction getAction() const { return m_eAction; }

    //!
    double getValue() const { return m_dValue; }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString			m_sName;
    EEventAction	m_eAction;
    double			m_dValue;
};

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CController : public CComponent
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor using a scene
    CController(C3DScene* pScene);

    //! Destructor
    virtual ~CController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setPositionTarget(QSP<CComponent> pComponent);

    //!
    void setRotationTarget(QSP<CComponent> pComponent);

    //!
    virtual void setMoveSpeed(double value) { m_dMoveSpeed = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QSP<CComponent> positionTarget() { return m_rPositionTarget.component(); }

    //!
    QSP<CComponent> rotationTarget() { return m_rRotationTarget.component(); }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void generateQ3DEvent(CQ3DEvent anEvent);

    //!
    void generateQ3DMouseEvent(CQ3DEvent::EEventAction eAction, QMouseEvent* event, CComponent* pComponent);

    //!
    virtual void keyPressEvent(QKeyEvent* event);

    //!
    virtual void keyReleaseEvent(QKeyEvent* event);

    //!
    virtual void mousePressEvent(QMouseEvent* event);

    //!
    virtual void mouseReleaseEvent(QMouseEvent* event);

    //!
    virtual void mouseMoveEvent(QMouseEvent* event);

    //!
    virtual void wheelEvent(QWheelEvent* event);

    //!
    virtual void joystickEvent(CJoystick* pJoystick);

    //!
    virtual void q3dEvent(CQ3DEvent* event);

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CController; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CJoystick*                          m_pJoystick;
    CComponentReference<CComponent>     m_rPositionTarget;
    CComponentReference<CComponent>     m_rRotationTarget;
    CComponentReference<CComponent>     m_rLookTarget;
    CQ3DEvent                           m_eCurrentLeftMouseEvent;
    CQ3DEvent                           m_eCurrentRightMouseEvent;
    CQ3DEvent                           m_eCurrentMiddleMouseEvent;
    QPoint                              m_pPreviousMousePos;
    Math::CRay3                         m_rLastRay;
    bool                                m_bUseMouse;
    bool                                m_bShiftPressed;
    bool                                m_bControlPressed;
    bool                                m_bAltPressed;
    double                              m_dMoveSpeed;
    double                              m_dForceFactor;
};
