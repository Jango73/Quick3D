
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
    void setCastShadows(bool value) { m_bCastShadows = value; }

    //!
    void setDistance(double value) { m_dDistance = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    CMaterial* getMaterial() const { return m_pMaterial; }

    //!
    bool castShadows() const { return m_bCastShadows; }

    //!
    double getDistance() const { return m_dDistance; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes h�rit�es
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return ClassName_CLight; }

    //! Charge les param�tres de cet objet
    virtual void loadParameters(CXMLNode xComponent);

    //! Est-ce que l'objet est une lumi�re?
    virtual bool isLight() const { return true; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    CLight& operator = (const CLight& target);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    double          m_dDistance;
    bool            m_bCastShadows;
    CMaterial*      m_pMaterial;
};
