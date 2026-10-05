
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QMap>
#include <QtOpenGL>
#include <QtOpenGL/QGLShaderProgram>

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
    void addShader(QString sName, QGLShaderProgram* value);

    //!
    QGLShaderProgram* getShader(QString sName);

protected:

    QMap<QString, QGLShaderProgram*> m_vShaders;
};
