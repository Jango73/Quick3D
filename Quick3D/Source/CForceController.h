
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CStandardController.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CForceController : public CStandardController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CForceController(C3DScene* pScene);

    //! Destructor
    virtual ~CForceController();

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
    virtual QString getClassName() const { return ClassName_CForceController; }

    //! Met l'objet à jour d'après le temps écoulé depuis la dernière image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void keyPressEvent(QKeyEvent* event);

    //!
    virtual void keyReleaseEvent(QKeyEvent *event);

    //!
    virtual void q3dEvent(CQ3DEvent* event);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

};
