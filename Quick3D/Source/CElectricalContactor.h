
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CElectricalComponent.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

class QUICK3D_EXPORT CElectricalContactor : public CElectricalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CElectricalContactor(C3DScene* pScene);

    //!
    virtual ~CElectricalContactor();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setClosed(bool bValue) { m_bClosed = bValue; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool closed() const { return m_bClosed; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CElectricalContactor; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void push(const CElectricalLoad& tLoad, double dDeltaTime);

    //!
    virtual CElectricalLoad pull(double dAmperage, double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool    m_bClosed;
};
