
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

CComponent* CLight::instantiator(C3DScene* pScene)
{
    return new CLight(pScene);
}

//-------------------------------------------------------------------------------------------------

CLight::CLight(C3DScene* pScene)
    : CCamera(pScene)
    , m_dLightingDistance(0.0)
    , m_dOcclusion(0.0)
    , m_bCastShadows(false)
{
    setRaytracable(false);

    m_pMaterial = QSP<CMaterial>(new CMaterial(pScene));
    m_pMaterial->createShadowTexture();
}

//-------------------------------------------------------------------------------------------------

CLight::CLight(const CLight& target)
    : CCamera(target)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CLight::~CLight()
{
}

//-------------------------------------------------------------------------------------------------

CLight& CLight::operator = (const CLight& target)
{
    CCamera::operator = (target);

    m_dLightingDistance = target.m_dLightingDistance;
    m_dOcclusion        = target.m_dOcclusion;
    m_bCastShadows      = target.m_bCastShadows;

    return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this light from \a xComponent.
*/
void CLight::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CCamera::loadParameters(sBaseFile, xComponent);

    CXMLNode tColorNode = xComponent.getNodeByTagName(ParamName_Color);

    if (m_pMaterial != nullptr)
    {
        if (tColorNode.isEmpty() == false)
        {
            m_pMaterial->diffuse().X = tColorNode.attributes()[ParamName_r].toDouble();
            m_pMaterial->diffuse().Y = tColorNode.attributes()[ParamName_g].toDouble();
            m_pMaterial->diffuse().Z = tColorNode.attributes()[ParamName_b].toDouble();
        }
    }

    CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

    if (xGeneralNode.isEmpty() == false)
    {
        if (xGeneralNode.attributes()[ParamName_Distance].isEmpty() == false)
        {
            m_dLightingDistance = xGeneralNode.attributes()[ParamName_Distance].toDouble();
        }

        if (xGeneralNode.attributes()[ParamName_Cast_Shadows].isEmpty() == false)
        {
            m_bCastShadows = (bool) xGeneralNode.attributes()[ParamName_Cast_Shadows].toInt();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CLight::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CLight]"));
    dumpIdent(stream, iIdent, QString("Lighting distance : %1").arg(m_dLightingDistance));
    dumpIdent(stream, iIdent, QString("Cast shadows : %1").arg(m_bCastShadows));

    CCamera::dump(stream, iIdent);
}
