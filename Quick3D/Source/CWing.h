
#pragma once

// qt-plus
#include "CInterpolator.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CPhysicalComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CWing : public CPhysicalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CWing(C3DScene* pScene);

    //!
    virtual ~CWing();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual void setAileronAngle_norm(double dValue);

    //!
    virtual void setAileronAngle_rad(double dAngle);

    //!
    virtual void setFlapsPosition_norm(double dValue);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    virtual double flapsPosition_norm() const { return m_dFlapsPosition_norm; }

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CWing; }

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

    double                  m_dArea_m2;
    double                  m_dWingAngle_rad;
    double                  m_dAileronArea_m2;
    double                  m_dAileronMaxPositiveAngle_rad;
    double                  m_dAileronAngle_rad;
    double                  m_dFlapsPosition_norm;
    Math::CVector3          m_vAileronPosition;
    CInterpolator<double>   m_iBodyAirflowDotLiftFactor;
    CInterpolator<double>   m_iBodyAirflowDotAileronLiftFactor;
};
