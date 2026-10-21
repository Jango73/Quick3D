
// qt-plus
#include "CLogger.h"
#include "CXMLNode.h"

// Application
#include "CEnvironment.h"
#include "CSkyModel.h"
#include "CSkyModelFactory.h"
#include "CEnvironmentDefaults.h"
#include "CAtmosphereOptics.h"
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CEnvironment with default values.
*/
CEnvironment::CEnvironment()
{
    resetToDefaults();
}

//-------------------------------------------------------------------------------------------------

/*!
    Resets the environment to default values.
*/
void CEnvironment::resetToDefaults()
{
    m_dAtmosphereAltitude = EnvironmentDefaults::AtmosphereAltitude;
    m_dFogMinimumDistance = EnvironmentDefaults::FogMinimumDistance;
    m_dUnderwaterFogDistance = EnvironmentDefaults::UnderwaterFogDistance;

    m_vGlobalAmbientColor = Math::CVector3(EnvironmentDefaults::GlobalAmbientR, EnvironmentDefaults::GlobalAmbientG, EnvironmentDefaults::GlobalAmbientB);
    m_vFogBaseColor = Math::CVector3(EnvironmentDefaults::FogColorR, EnvironmentDefaults::FogColorG, EnvironmentDefaults::FogColorB);
    m_vSkyBaseColor = Math::CVector3(EnvironmentDefaults::SkyColorR, EnvironmentDefaults::SkyColorG, EnvironmentDefaults::SkyColorB);

    m_pSkyModel = CSkyModelFactory::create(SkyModels::DefaultName);
    m_dOzoneAmount = EnvironmentDefaults::OzoneAmount;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the environment from an \a xEnvironment XML node.
    Missing attributes or colors keep their current values.
*/
void CEnvironment::loadFromXml(const CXMLNode& xEnvironment)
{
    readDoubleAttribute(xEnvironment, ParamName_AtmosphereAltitude, m_dAtmosphereAltitude);
    readDoubleAttribute(xEnvironment, ParamName_FogMinimumDistance, m_dFogMinimumDistance);
    readDoubleAttribute(xEnvironment, ParamName_UnderwaterFogDistance, m_dUnderwaterFogDistance);

    if (xEnvironment.attributes()[ParamName_SkyModel].isEmpty() == false)
    {
        setSkyModel(xEnvironment.attributes()[ParamName_SkyModel]);
    }

    double dOzoneAmount = m_dOzoneAmount;

    if (readDoubleAttribute(xEnvironment, ParamName_OzoneAmount, dOzoneAmount) == true)
    {
        setOzoneAmount(dOzoneAmount);
    }

    readColorAttribute(xEnvironment, ParamName_GlobalAmbient, m_vGlobalAmbientColor);
    readColorAttribute(xEnvironment, ParamName_FogColor, m_vFogBaseColor);
    readColorAttribute(xEnvironment, ParamName_SkyColor, m_vSkyBaseColor);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the sky model matching \a sModel. Unknown names are rejected, the
    current model stays active.
*/
void CEnvironment::setSkyModel(const QString& sModel)
{
    QSP<CSkyModel> pNewModel = CSkyModelFactory::create(sModel);

    if (pNewModel != nullptr)
    {
        m_pSkyModel = pNewModel;
    }
    else
    {
        LOG_WARNING(QString("CEnvironment::setSkyModel() : unknown sky model %1, keeping %2").arg(sModel).arg(skyModelName()));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the ozone column amount scaling the nominal ozone absorption to \a value.
    Negative values are clipped to zero, which disables ozone absorption.
*/
void CEnvironment::setOzoneAmount(double value)
{
    m_dOzoneAmount = CAtmosphereOptics::clampOzoneAmount(value);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the selected sky model name, empty when no model is set.
*/
QString CEnvironment::skyModelName() const
{
    if (m_pSkyModel == nullptr)
    {
        return QString();
    }

    return m_pSkyModel->name();
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the shader id matching the selected sky model, -1 when none is set.
*/
int CEnvironment::skyModelId() const
{
    if (m_pSkyModel == nullptr)
    {
        return -1;
    }

    return m_pSkyModel->id();
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads the double XML attribute \a sName of \a xNode into \a dValue, returns true when present.
*/
bool CEnvironment::readDoubleAttribute(const CXMLNode& xNode, const QString& sName, double& dValue)
{
    if (xNode.attributes()[sName].isEmpty() == false)
    {
        dValue = xNode.attributes()[sName].toDouble();
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads the \a sTag color child of \a xEnvironment into \a vColor.
*/
void CEnvironment::readColorAttribute(const CXMLNode& xEnvironment, const QString& sTag, Math::CVector3& vColor)
{
    CXMLNode xColor = xEnvironment.getNodeByTagName(sTag);

    if (xColor.isEmpty() == false)
    {
        if (xColor.attributes()[ParamName_r].isEmpty() == false)
        {
            vColor.X = xColor.attributes()[ParamName_r].toDouble();
        }

        if (xColor.attributes()[ParamName_g].isEmpty() == false)
        {
            vColor.Y = xColor.attributes()[ParamName_g].toDouble();
        }

        if (xColor.attributes()[ParamName_b].isEmpty() == false)
        {
            vColor.Z = xColor.attributes()[ParamName_b].toDouble();
        }
    }
}
