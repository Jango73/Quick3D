
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CEngine : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CEngine(C3DScene* pScene);

    //!
    virtual ~CEngine();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setMaxThrust_kg(double value);

    //!
    virtual void setCurrentFuelFlow_norm(double value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double currentFuelFlow_norm() const;

    //!
    virtual double currentThrust_kg() const;

    //!
    virtual double currentFuelCons_ls() const;

    //!
    virtual bool alternatorActive() const;

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CEngine; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Updates this object using the elapsed time since last update
    virtual void update(double dDeltaTimeS) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double m_dMaxThrust_kg;
    double m_dFuelFlow_norm;
};
