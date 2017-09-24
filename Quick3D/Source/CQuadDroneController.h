
#pragma once

#include "quick3d_global.h"

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// qt-plus
#include "CInterpolator.h"

// Application
#include "CQ3DConstants.h"
#include "CAircraftController.h"
#include "CComponentReference.h"
#include "CWing.h"
#include "CElevator.h"
#include "CRudder.h"
#include "CEngine.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CQuadDroneController : public CAircraftController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CQuadDroneController(C3DScene* pScene);

    //!
    virtual ~CQuadDroneController();

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
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CQuadDroneController; }

    //!
    virtual void update(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CInterpolator<Math::CVector4>   m_iThrustModifiers_Forward;
    CInterpolator<Math::CVector4>   m_iThrustModifiers_Lateral;
    CInterpolator<Math::CVector4>   m_iThrustModifiers_Yaw;
    double                          m_dMainThrust;
};
