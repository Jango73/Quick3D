
#pragma once

// Qt
#include <QString>
#include <QVector3D>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QOpenGLShaderProgram;

//-------------------------------------------------------------------------------------------------

/*!
    Base class of sky rendering models.

    A sky model pushes its shader uniforms through setupUniforms(). The base
    version only pushes the model id and does nothing special. Derived models
    add their own uniforms. Use create() to build the model matching a scene
    XML name.
*/
class QUICK3D_EXPORT CSkyModel
{
public:

    //! Destructor
    virtual ~CSkyModel();

    //! Returns the model name as written in scene XML files
    virtual QString name() const = 0;

    //! Returns the shader id selecting this model
    virtual int id() const = 0;

    //! Pushes the model uniforms for the given sun direction and ozone amount
    virtual void setupUniforms(QOpenGLShaderProgram* pProgram, const QVector3D& vSunDirection, double dOzoneAmount);

    //! Creates the model matching \a sName, nullptr when unknown
    static CSkyModel* create(const QString& sName);
};
