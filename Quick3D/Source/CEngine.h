
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

class QUICK3D_EXPORT CEngine : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

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

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CEngine; }

    //!
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

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
