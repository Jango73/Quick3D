
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CEngine.h"
#include "CServoPosition.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CJetEngine : public CEngine
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CJetEngine(C3DScene* pScene);

    //!
    virtual ~CJetEngine();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns thrust in kilograms
    virtual double currentThrust_kg() const Q_DECL_OVERRIDE;

    //! Returns fuel consumption in liters per second
    virtual double currentFuelCons_ls() const Q_DECL_OVERRIDE;

    //! Returns \c true if alternator is active
    virtual bool alternatorActive() const Q_DECL_OVERRIDE;

    //! Returns normalized N1
    virtual double n1_norm() const;

    //! Returns normalized N2
    virtual double n2_norm() const;

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CJetEngine; }

    //! Updates this object
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CServoPosition  m_dN1_servo;
    double          m_dN2_norm;
};
