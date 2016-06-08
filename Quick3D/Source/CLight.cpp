
// Application
#include "CLight.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CLight
    \brief The base class for all lights existing in the Quick3D engine.
    \inmodule Quick3D
    \sa C3DScene
*/

//-------------------------------------------------------------------------------------------------

CComponent* CLight::instanciator(C3DScene* pScene)
{
    return new CLight(pScene);
}

//-------------------------------------------------------------------------------------------------

CLight::CLight(C3DScene* pScene)
    : CCamera(pScene)
    , m_dDistance(0.0)
    , m_bCastShadows(false)
    , m_pMaterial(NULL)
{
    setRaytracable(false);

    m_pMaterial = new CMaterial(pScene);
    m_pMaterial->createShadowTexture();
}

//-------------------------------------------------------------------------------------------------

CLight::CLight(const CLight& target)
    : CCamera(target)
    , m_pMaterial(NULL)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CLight::~CLight()
{
    if (m_pMaterial != NULL)
    {
        delete m_pMaterial;
    }
}

//-------------------------------------------------------------------------------------------------

CLight& CLight::operator = (const CLight& target)
{
    CCamera::operator = (target);

    m_dDistance         = target.m_dDistance;
    m_bCastShadows      = target.m_bCastShadows;

    if (m_pMaterial != NULL && target.m_pMaterial != NULL)
    {
        // *m_pMaterial = *target.m_pMaterial;
    }

    return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this light from \a xComponent.
*/
void CLight::loadParameters(CXMLNode xComponent)
{
    CCamera::loadParameters(xComponent);

    CXMLNode tColorNode = xComponent.getNodeByTagName(ParamName_Color);

    if (m_pMaterial != NULL)
    {
        if (tColorNode.isEmpty() == false)
        {
            m_pMaterial->getDiffuse().X = tColorNode.attributes()[ParamName_r].toDouble();
            m_pMaterial->getDiffuse().Y = tColorNode.attributes()[ParamName_g].toDouble();
            m_pMaterial->getDiffuse().Z = tColorNode.attributes()[ParamName_b].toDouble();
        }
    }

    CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

    if (xGeneralNode.isEmpty() == false)
    {
        if (xGeneralNode.attributes()[ParamName_Distance].isEmpty() == false)
        {
            m_dDistance = xGeneralNode.attributes()[ParamName_Distance].toDouble();
        }

        if (xGeneralNode.attributes()[ParamName_Cast_Shadows].isEmpty() == false)
        {
            m_bCastShadows = (bool) xGeneralNode.attributes()[ParamName_Cast_Shadows].toInt();
        }
    }
}
