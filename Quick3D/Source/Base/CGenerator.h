
#pragma once

// Application
#include "quick3d_global.h"
#include "CComponentReference.h"
#include "CElectricalComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGenerator : public CElectricalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CGenerator(C3DScene* pScene);

    //!
    virtual ~CGenerator();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CElectricalLoad& current();

    //!
    const CElectricalLoad& current() const;

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CEngineGenerator; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CElectricalLoad                 m_tCurrent;
};