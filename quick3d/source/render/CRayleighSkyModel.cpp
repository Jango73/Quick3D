
// Application
#include "CRayleighSkyModel.h"
#include "CAtmosphereOptics.h"
#include "CEnvironmentDefaults.h"

// Qt
#include <QOpenGLShaderProgram>

//-------------------------------------------------------------------------------------------------

/*!
    Pushes the model uniforms for the given sun direction and ozone amount.
*/
void CRayleighSkyModel::setupUniforms(QOpenGLShaderProgram* pProgram, const QVector3D& vSunDirection, double dOzoneAmount)
{
    CSkyModel::setupUniforms(pProgram, vSunDirection, dOzoneAmount);

    dOzoneAmount = CAtmosphereOptics::clampOzoneAmount(dOzoneAmount);

    pProgram->setUniformValue(SkyUniforms::SunDirection, vSunDirection);
    pProgram->setUniformValue(SkyUniforms::RayleighBeta, QVector3D(AtmospherePhysics::RayleighBetaR, AtmospherePhysics::RayleighBetaG, AtmospherePhysics::RayleighBetaB));
    pProgram->setUniformValue(SkyUniforms::RayleighScaleHeight, (GLfloat) AtmospherePhysics::RayleighScaleHeight);
    pProgram->setUniformValue(SkyUniforms::OzoneBeta, QVector3D(AtmospherePhysics::OzoneBetaR, AtmospherePhysics::OzoneBetaG, AtmospherePhysics::OzoneBetaB));
    pProgram->setUniformValue(SkyUniforms::OzoneAmount, (GLfloat) dOzoneAmount);
    pProgram->setUniformValue(SkyUniforms::SkyExposure, (GLfloat) EnvironmentDefaults::SkyExposure);
}
