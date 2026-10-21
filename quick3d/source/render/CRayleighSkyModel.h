
#pragma once

// Application
#include "CSkyModel.h"
#include "CEnvironmentDefaults.h"

//-------------------------------------------------------------------------------------------------

/*!
    Rayleigh single scattering sky model.

    Computes the sky from the same Rayleigh scattering and ozone absorption as
    the sun tint: sunlight transmittance from u_sun_color, view path from the
    view elevation, scattering share and view extinction from the ozone amount,
    Rayleigh phase from the sun angle. The zenith, horizon and sunset glow
    emerge from the computation, no keyframed colors.
*/
class QUICK3D_EXPORT CRayleighSkyModel : public CSkyModel
{
public:

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the model name as written in scene XML files
    QString name() const Q_DECL_OVERRIDE { return SkyModels::PhysicalName; }

    //! Returns the shader id selecting this model
    int id() const Q_DECL_OVERRIDE { return SkyModels::PhysicalId; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Pushes the model uniforms for the given sun direction and ozone amount
    void setupUniforms(QOpenGLShaderProgram* pProgram, const QVector3D& vSunDirection, double dOzoneAmount) Q_DECL_OVERRIDE;
};
