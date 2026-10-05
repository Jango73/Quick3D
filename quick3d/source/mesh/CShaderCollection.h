
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QMap>
#include <QtOpenGL>
#include <QtOpenGL/QOpenGLShaderProgram>

// Application
#include "CQ3DConstants.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CShaderCollection
{
public:

    //!
    CShaderCollection();

    //!
    ~CShaderCollection();

    //!
    void clear();

    //!
    void addShader(QString sName, QOpenGLShaderProgram* value);

    //!
    QOpenGLShaderProgram* getShader(QString sName);

protected:

    QMap<QString, QOpenGLShaderProgram*> m_vShaders;
};
