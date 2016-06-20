
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CComponentReference.h"
#include "CJoystick.h"

//-------------------------------------------------------------------------------------------------

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
    // Constructeurs et destructeur
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
    // Propriétés
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
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur d'après une scène
    CController(C3DScene* pScene);

    //! Destructeur
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
    QSP<CComponent> getPositionTarget() { return m_rPositionTarget.component(); }

    //!
    QSP<CComponent> getRotationTarget() { return m_rRotationTarget.component(); }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
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
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de l'objet
    virtual QString getClassName() const { return ClassName_CController; }

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent);

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene);

    //!
    virtual void update(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    CJoystick*							m_pJoystick;
    CComponentReference<CComponent>		m_rPositionTarget;
    CComponentReference<CComponent>		m_rRotationTarget;
    CQ3DEvent							m_eCurrentLeftMouseEvent;
    CQ3DEvent							m_eCurrentRightMouseEvent;
    CQ3DEvent							m_eCurrentMiddleMouseEvent;
    QPoint								m_pPreviousMousePos;
    Math::CRay3                         m_rLastRay;
    bool								m_bUseMouse;
    bool								m_bControlPressed;
    bool								m_bAltPressed;
    double								m_dMoveSpeed;
    double								m_dForceFactor;
};
