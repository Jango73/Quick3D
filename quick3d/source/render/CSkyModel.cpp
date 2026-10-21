
// Application
#include "CSkyModel.h"
#include "CRayleighSkyModel.h"
#include "CEnvironmentDefaults.h"

// Qt
#include <QOpenGLShaderProgram>

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a sky model.
*/
CSkyModel::~CSkyModel()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Pushes the model uniforms for the given sun direction and ozone amount.
    The base version only pushes the model id and does nothing special.
*/
void CSkyModel::setupUniforms(QOpenGLShaderProgram* pProgram, const QVector3D& vSunDirection, double dOzoneAmount)
{
    Q_UNUSED(vSunDirection);
    Q_UNUSED(dOzoneAmount);

    pProgram->setUniformValue("u_sky_model", (GLint) id());
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the model matching \a sName, nullptr when unknown.
*/
CSkyModel* CSkyModel::create(const QString& sName)
{
    if (sName == SKY_MODEL_PHYSICAL)
    {
        return new CRayleighSkyModel();
    }

    return nullptr;
}
