
#pragma once

// Qt
#include <QGLWidget>
#include <QImage>
#include <QTime>

// qt-plus
#include "CInterpolator.h"

// Application
#include "quick3d_global.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "C3DScene.h"
#include "CMeshGeometry.h"

//-------------------------------------------------------------------------------------------------

#define MAX_GL_LIGHTS   8

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CGLWidgetScene : public QGLWidget, public C3DScene
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CGLWidgetScene(bool bForDisplay = true);

    //! Destructor
    virtual ~CGLWidgetScene();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Render
    virtual void paintGL();

    //!
    virtual void init(QVector<QSP<CComponent> > vComponents);

    //!
    virtual void initShaders();

    //!
    virtual void setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem);

    //!
    virtual void setupLights(CRenderContext* pContext);

    //!
    virtual void computeLightsOcclusion(CRenderContext* pContext);

    //!
    virtual void makeCurrentRenderingContext();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Try to add a shader program composed of vertex, geometry and fragment shading code
    bool tryAddShaderProgram_VGF(const QString& sName, const QString& sVertexPath, const QString& sGeometryPath, const QString& sFragmentPath);

    //! Try to add a shader program composed of vertex and fragment shading code
    bool tryAddShaderProgram_VF(const QString& sName, const QString& sVertexPath, const QString& sFragmentPath);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    GLint           iOpenGLLightIndex;
    GLint           u_shadow_enable;
    GLint           u_light_is_sun [MAX_GL_LIGHTS];
    QVector3D       u_light_position [MAX_GL_LIGHTS];
    QVector3D       u_light_screen_position [MAX_GL_LIGHTS];
    QVector3D       u_light_direction [MAX_GL_LIGHTS];
    QVector3D       u_light_color [MAX_GL_LIGHTS];
    GLfloat         u_light_distance_to_camera [MAX_GL_LIGHTS];
    GLfloat         u_light_distance [MAX_GL_LIGHTS];
    GLfloat         u_light_spot_angle [MAX_GL_LIGHTS];
    GLfloat         u_light_occlusion [MAX_GL_LIGHTS];
    QVector3D       vSunColor;
};
