
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

class QUICK3D_EXPORT CGLWidgetScene : public QGLWidget, public C3DScene
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
    CGLWidgetScene(bool bForDisplay = true);

    //! Destructeur
    virtual ~CGLWidgetScene();

    //-------------------------------------------------------------------------------------------------
    // Méthodes héritées
    //-------------------------------------------------------------------------------------------------

    //! Rendu
    virtual void paintGL();

    //!
    virtual void init(QVector<CComponent*> vComponents);

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
    // Propriétés
    //-------------------------------------------------------------------------------------------------

protected:
};

#endif // CGLWIDGETSCENE_H
