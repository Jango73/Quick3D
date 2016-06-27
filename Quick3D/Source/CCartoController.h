
#pragma once

#include "quick3d_global.h"

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "CQ3DConstants.h"
#include "CController.h"

class C3DScene;

class QUICK3D_EXPORT CCartoController : public CController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'après une scène
    CCartoController(C3DScene* pScene);

    //! Destructeur
    virtual ~CCartoController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CCartoController; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void keyPressEvent(QKeyEvent* event);

    //!
    virtual void keyReleaseEvent(QKeyEvent *event);

    //!
    virtual void mousePressEvent(QMouseEvent* event);

    //!
    virtual void mouseReleaseEvent(QMouseEvent* event);

    //!
    virtual void mouseMoveEvent(QMouseEvent* event);

    //!
    virtual void wheelEvent(QWheelEvent *event);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    QPoint	m_pPreviousMousePos;
    bool	m_bUseMouse;
    bool	m_bPanTilt;
};
