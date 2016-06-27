
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Fondations
#include "CVector2.h"
#include "CVector3.h"
#include "CMaterial.h"
#include "CCamera.h"

//-------------------------------------------------------------------------------------------------
// Déclarations avancées
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CLight : public CCamera
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
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
    double getLightingDistance() const { return m_dLightingDistance; }

    //!
    QSP<CMaterial> getMaterial() const { return m_pMaterial; }

    //!
    double getOcclusion() const { return m_dOcclusion; }

    //!
    bool castShadows() const { return m_bCastShadows; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CLight; }

    //! Charge les paramètres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Est-ce que l'objet est une lumière?
    virtual bool isLight() const { return true; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    CLight& operator = (const CLight& target);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:

    double          m_dLightingDistance;
    double          m_dOcclusion;
    bool            m_bCastShadows;

    // Shared data

    QSP<CMaterial>  m_pMaterial;
};
