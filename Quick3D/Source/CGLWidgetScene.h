
#ifndef CGLWIDGETSCENE_H
#define CGLWIDGETSCENE_H

#include "quick3d_global.h"

// Qt
#include <QGLWidget>
#include <QImage>
#include <QTime>

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "Interpolator.h"

// Application
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

    //! Constructeur
    CGLWidgetScene(bool bForDisplay = true);

    //! Destructor
    virtual ~CGLWidgetScene();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Rendu
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

#endif // CGLWIDGETSCENE_H
