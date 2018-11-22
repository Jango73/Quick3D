
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Application
#include "CQ3DConstants.h"
#include "CController.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CStandardController : public CController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CStandardController(C3DScene* pScene);

    //! Destructor
    virtual ~CStandardController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CStandardController; }

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    //!
    virtual void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    //!
    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    //!
    virtual void q3dEvent(CQ3DEvent* event) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool		m_bLookUp;
    bool		m_bLookDown;
    bool		m_bGoForward;
    bool		m_bGoBackward;
    bool		m_bGoUp;
    bool		m_bGoDown;
    bool		m_bTurnRight;
    bool		m_bTurnLeft;
    bool		m_bStrafeRight;
    bool		m_bStrafeLeft;
    bool		m_bAltitudeFastUp;
    bool		m_bAltitudeFastDown;
};
