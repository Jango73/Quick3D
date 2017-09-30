
#pragma once

// Application
#include "quick3d_global.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CMaterial.h"
#include "CCamera.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CLight : public CCamera
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CLight(C3DScene* pScene);

    //! Copy constructor
    CLight(const CLight&);

    //! Destructor
    virtual ~CLight();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setOcclusion(double value) { m_dOcclusion = value; }

    //!
    void setCastShadows(bool value) { m_bCastShadows = value; }

    //!
    void setLightingDistance(double value) { m_dLightingDistance = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    double lightingDistance() const { return m_dLightingDistance; }

    //!
    QSP<CMaterial> material() const { return m_pMaterial; }

    //!
    double occlusion() const { return m_dOcclusion; }

    //!
    bool castShadows() const { return m_bCastShadows; }

    //-------------------------------------------------------------------------------------------------
    // Overridden methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CLight; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CLight& operator = (const CLight& target);

    //! Returns \c true if this object is a light
    virtual bool isLight() const { return true; }

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double          m_dLightingDistance;
    double          m_dOcclusion;
    bool            m_bCastShadows;

    // Shared data

    QSP<CMaterial>  m_pMaterial;
};
