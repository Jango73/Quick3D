
// Application
#include "CRayleighSkyModel.h"
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

    if (dOzoneAmount < 0.0)
    {
        dOzoneAmount = 0.0;
    }

    pProgram->setUniformValue("u_sun_direction", vSunDirection);
    pProgram->setUniformValue("u_rayleigh_beta", QVector3D(DEFAULT_RAYLEIGH_BETA_R, DEFAULT_RAYLEIGH_BETA_G, DEFAULT_RAYLEIGH_BETA_B));
    pProgram->setUniformValue("u_rayleigh_scale_height", (GLfloat) DEFAULT_RAYLEIGH_SCALE_HEIGHT);
    pProgram->setUniformValue("u_ozone_beta", QVector3D(DEFAULT_OZONE_BETA_R, DEFAULT_OZONE_BETA_G, DEFAULT_OZONE_BETA_B));
    pProgram->setUniformValue("u_ozone_amount", (GLfloat) dOzoneAmount);
    pProgram->setUniformValue("u_sky_exposure", (GLfloat) DEFAULT_SKY_EXPOSURE);
}
