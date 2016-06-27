
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

    //! Constructeur d'apr�s une sc�ne
    CCartoController(C3DScene* pScene);

    //! Destructeur
    virtual ~CCartoController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
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
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    QPoint	m_pPreviousMousePos;
    bool	m_bUseMouse;
    bool	m_bPanTilt;
};
