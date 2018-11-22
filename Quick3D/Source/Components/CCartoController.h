
#pragma once

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CController.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CCartoController : public CController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CCartoController(C3DScene* pScene);

    //! Destructor
    virtual ~CCartoController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CCartoController; }

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
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QPoint      m_pPreviousMousePos;
    bool        m_bUseMouse;
    bool        m_bPanTilt;
};
