
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Fondations
#include "CVector2.h"
#include "CVector3.h"
#include "CMaterial.h"
#include "CCamera.h"

//-------------------------------------------------------------------------------------------------
// D�clarations avanc�es
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CLight : public CCamera
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //!
    CLight(C3DScene* pScene);

    //!
    CLight(const CLight&);

    //!
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
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CLight; }

    //! Loads this object's parameters
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Est-ce que l'objet est une lumi�re?
    virtual bool isLight() const { return true; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CLight& operator = (const CLight& target);

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
