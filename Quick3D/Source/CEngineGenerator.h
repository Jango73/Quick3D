
#pragma once

// Application
#include "quick3d_global.h"
#include "CComponentReference.h"
#include "CElectricalComponent.h"
#include "CEngine.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CEngineGenerator : public CElectricalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CEngineGenerator(C3DScene* pScene);

    //!
    virtual ~CEngineGenerator();

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

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Solves the links of this object
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Deletes this object's links
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CComponentReference<CEngine>    m_rEngineTarget;
    CElectricalLoad                 m_tCurrent;
};
