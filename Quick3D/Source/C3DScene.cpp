
#ifndef WIN32
#include <sys/param.h>
#endif

// Qt
#include <QtOpenGL>
#include <GL/glu.h>

// Fondations
#include "CLogManager.h"

// Application
#include "C3DScene.h"
#include "CView.h"
#include "CRessourcesManager.h"
#include "COBJLoader.h"
#include "CMaterial.h"
#include "CHeightField.h"
#include "CWorldTerrain.h"
#include "CTrajectorable.h"
#include "CController.h"
#include "CStandardController.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class C3DScene
    \brief The base class for a scene that holds 3D objects.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a C3DScene with its default parameters.
    \a bForDisplay tells if the scene will be displayed
*/
C3DScene::C3DScene(bool bForDisplay)
    : m_pRessourcesManager(NULL)
    , m_pBuildingGenerator(NULL)
    , m_pTreeGenerator(NULL)
    , m_vShaders(NULL)
    , m_pController(NULL)
    , m_pDefaultController(NULL)
    , m_bForDisplay(bForDisplay)
    , m_bFrustumCheck(true)
    , m_bEditMode(false)
    , m_DebugMode(false)
    , m_bBoundsOnly(false)
    , m_bNormalsOnly(false)
    , m_dShaderQuality(0.5)
    , m_pRain(NULL)
    , m_tTimeOfDay(12, 0, 0)
    , m_dTime(0.0)
    , m_dWindLevel(0.5)
    , m_bRenderingShadows(false)
    , m_bforceWideFOV(false)
    , m_bforceSmallFOV(false)
    , m_bForceIR(false)
    , m_bStreamView(false)
    , m_bDepthComputing(false)
{
    LOG_DEBUG("C3DScene::C3DScene()");

    m_pSegments = QSP<CMeshGeometry>(new CMeshGeometry(this));
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a C3DScene.
*/
C3DScene::~C3DScene()
{
    LOG_DEBUG("C3DScene::~C3DScene()");

    clearComponents();
    clearViewports();

    if (m_pGLExtension != NULL) delete m_pGLExtension;
    if (m_vShaders != NULL) delete m_vShaders;
    if (m_pTreeGenerator != NULL) delete m_pTreeGenerator;
    if (m_pBuildingGenerator != NULL) delete m_pBuildingGenerator;
    if (m_pRessourcesManager != NULL) delete m_pRessourcesManager;
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components, lights and viewports in the scene.
*/
void C3DScene::clear()
{
    clearComponents();
    clearViewports();

    m_pRessourcesManager->clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all components in the scene.
*/
void C3DScene::clearComponents()
{
    // Destruction des composants

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->clearLinks(this);
    }

    m_vComponents.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes all viewports in the scene.
*/
void C3DScene::clearViewports()
{
    // Destruction des viewports

    foreach (int iIndex, m_pViewports.keys())
    {
        delete m_pViewports[iIndex];
    }

    m_pViewports.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes the scene using components in \a vComponents.
*/
void C3DScene::init(QVector<QSP<CComponent> > vComponents)
{
    LOG_DEBUG("C3DScene::init()");

    //-----------------------------------------------
    // Init fog and sun color

    m_tFog.enabled() = true;
    m_tFog.level() = 0.0;
    m_tFog.color() = CVector3(0.5, 0.5, 0.5);

    m_vSunColor.clear();
    m_vSunColor.addValue( 0.00, Vector4(0.0, 0.0, 0.0, 1.0));
    m_vSunColor.addValue( 0.50, Vector4(1.1, 0.7, 0.2, 1.0));
    m_vSunColor.addValue( 0.60, Vector4(1.1, 0.7, 0.2, 1.0));
    m_vSunColor.addValue( 0.75, Vector4(1.4, 1.4, 1.2, 1.0));
    m_vSunColor.addValue( 1.00, Vector4(1.4, 1.4, 1.2, 1.0));

    //-----------------------------------------------
    // Create particles

    // m_pRain = new CRain(this);

    //-----------------------------------------------
    // Assign components

    m_vComponents = vComponents;

    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->addItems(this);
    }

    //-----------------------------------------------
    // Add sun

    QVector<QSP<CLight> > vLights = lights();
    bool bFoundSun = false;

    foreach (QSP<CLight> pLight, vLights)
    {
        if (pLight->getTag() == "SUN")
        {
            bFoundSun = true;
            break;
        }
    }

    if (bFoundSun == false)
    {
        QSP<CLight> pLight = QSP<CLight>(new CLight(this));

        pLight->setName("SUN");
        pLight->setTag("SUN");
        pLight->setCastShadows(true);
        pLight->setFOV(10.0);

        m_vComponents.append(pLight);
    }

    //-----------------------------------------------

    /*
    if (m_pController != NULL)
    {
        m_pController->solveLinks(this);
    }
    */

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->solveLinks(this);
    }

    autoResolveHeightFields();
}

//-------------------------------------------------------------------------------------------------

/*!
    Initializes shaders.
*/
void C3DScene::initShaders()
{
    LOG_DEBUG("C3DScene::initShaders()");
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the world origin to \a value. \br\br
    This property is used as a center point when rendering, in order to avoid huge geocentric position values in matrices and shaders, which lead to visual artifacts.
*/
void C3DScene::setWorldOrigin(Math::CVector3 value)
{
    m_vWorldOrigin = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the active controller of the scene to \a pController.
*/
void C3DScene::setController(CController* pController)
{
    if (pController != NULL)
    {
        m_pController = pController;
    }
    else
    {
        m_pController = m_pDefaultController;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the default controller of the scene to \a pController.
*/
void C3DScene::setDefaultController(CController* pController)
{
    m_pDefaultController = pController;
    m_pController = pController;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the shader quality of the scene to \a value. \br\br
    This value is used in shaders in order to easily adjust the shader level of complexity.
*/
void C3DScene::setShaderQuality(double value)
{
    m_dShaderQuality = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value, expressed in seconds. \br\br
    The time property is mainly used by animated shaders.
*/
void C3DScene::setTime(double value)
{
    m_dTime = value;

    CMaterial::setTime(value);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the time of the scene to \a value. \br\br
    The time of day property is mainly used for sunlight computation.
*/
void C3DScene::setTimeOfDay(QTime value)
{
    m_tTimeOfDay = value;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setFogLevel(double value)
{
    m_tFog.level() = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the force of the wind to \a value.
*/
void C3DScene::setWindLevel(double value)
{
    m_dWindLevel = value;
    if (m_dWindLevel < 0.05) m_dWindLevel = 0.05;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::setRainVisible(bool bOnOff)
{
    if (m_pRain) m_pRain->setVisible(bOnOff);
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces a wide FOV when rendering when \a value is \c true.
*/
void C3DScene::forceWideFOV(bool value)
{
    m_bforceWideFOV = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces a small FOV when rendering when \a value is \c true.
*/
void C3DScene::forceSmallFOV(bool value)
{
    m_bforceSmallFOV = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Forces IR effect when rendering.
*/
void C3DScene::forceIR(bool value)
{
    m_bForceIR = value;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of all lights.
*/
QVector<QSP<CLight> > C3DScene::lights()
{
    QVector<QSP<CLight> > vLights;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        getLightsRecurse(vLights, pComponent);
    }

    return vLights;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::getLightsRecurse(QVector<QSP<CLight> >& vLights, QSP<CComponent> pComponent)
{
    if (pComponent->isLight())
    {
        QSP<CLight> pLight = QSP_CAST(CLight, pComponent);

        if (pLight)
        {
            vLights.append(pLight);
        }
    }

    foreach (QSP<CComponent> pChild, pComponent->getChildren())
    {
        getLightsRecurse(vLights, pChild);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of components whose tag matches \a sTag.
*/
QVector<QSP<CComponent> > C3DScene::componentsByTag(const QString& sTag)
{
    QVector<QSP<CComponent> > vReturnValue;

    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->getTag() == sTag)
        {
            vReturnValue.append(pComponent);
        }
    }

    return vReturnValue;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a vector of lights whose tag matches \a sTag.
*/
QVector<QSP<CLight> > C3DScene::lightsByTag(const QString& sTag)
{
    QVector<QSP<CLight> > vLights;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        getLightsByTagRecurse(vLights, sTag, pComponent);
    }

    return vLights;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::getLightsByTagRecurse(QVector<QSP<CLight> >& vLights, const QString& sTag, QSP<CComponent> pComponent)
{
    if (pComponent->isLight() && pComponent->getTag() == sTag)
    {
        QSP<CLight> pLight = QSP_CAST(CLight, pComponent);

        if (pLight)
        {
            vLights.append(pLight);
        }
    }

    foreach (QSP<CComponent> pChild, pComponent->getChildren())
    {
        getLightsByTagRecurse(vLights, sTag, pChild);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates the scene using elapsed time in \a dDeltaTimeS.
*/
void C3DScene::updateScene(double dDeltaTimeS)
{
    if (dDeltaTimeS < 0.0) dDeltaTimeS = 0.0;
    if (dDeltaTimeS > 1.0) dDeltaTimeS = 1.0;

    if (m_bEditMode == false)
    {
        m_pSegments->clear();
    }

    if (m_pController != NULL)
    {
        m_pController->update(dDeltaTimeS);
    }

    if (m_bEditMode)
    {
        dDeltaTimeS = 0.0;
    }

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->update(dDeltaTimeS);
    }

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        pComponent->postUpdate(dDeltaTimeS);
    }

    CPhysicalComponent::computeCollisions(m_vComponents, dDeltaTimeS);
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints all components, using \a pContext.
*/
void C3DScene::paintComponents(CRenderContext* pContext)
{
    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->isVisible())
        {
            pComponent->paint(pContext);
            pComponent->postPaint(pContext);
        }
    }

#ifdef DRAW_BY_MATERIAL
    pContext->meshByMaterial()->paint(pContext);
#endif

    m_pSegments->paint(pContext, NULL);
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints all components that cast shadows, using \a pContext.
*/
void C3DScene::paintShadowCastingComponents(CRenderContext* pContext)
{
    foreach(QSP<CComponent> pComponent, m_vComponents)
    {
        if (pComponent->isVisible() && pComponent->castsShadows())
        {
            pComponent->paint(pContext);
            pComponent->postPaint(pContext);
        }
    }

#ifdef DRAW_BY_MATERIAL
    pContext->meshByMaterial()->paint(pContext);
#endif
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets up the environment of the scene.
*/
void C3DScene::setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets up the lights of the scene.
*/
void C3DScene::setupLights(CRenderContext* pContext)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Makes the OpenGL context of this scene the current one.
*/
void C3DScene::makeCurrentRenderingContext()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds \a pComponent to the scene.
*/
void C3DScene::addComponent(QSP<CComponent> pComponent)
{
    m_vComponents.append(pComponent);
    pComponent->solveLinks(this);
    autoResolveHeightFields();
}

//-------------------------------------------------------------------------------------------------

void C3DScene::autoResolveHeightFields()
{
    CHeightField* pTerrain = NULL;

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        if (dynamic_cast<CWorldTerrain*>(pComponent.data()) != NULL)
        {
            pTerrain = dynamic_cast<CHeightField*>(pComponent.data());
        }
        else
        {
            if (dynamic_cast<CTrajectorable*>(pComponent.data()) != NULL)
            {
                CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pComponent.data());

                if (pPhysical != NULL && pTerrain != NULL)
                {
                    pPhysical->addField(pTerrain);
                }
            }
        }

        pComponent->update(0.0);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Deletes all components in the scene that have a tag equal to \a sTag.
*/
void C3DScene::deleteComponentsByTag(const QString& sTag)
{
    for (int iIndex = 0; iIndex < m_vComponents.count(); iIndex++)
    {
        if (m_vComponents[iIndex]->getTag() == sTag)
        {
            m_vComponents[iIndex]->clearLinks(this);
            m_vComponents.remove(iIndex);
            iIndex--;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects components in the scene.
*/
RayTracingResult C3DScene::intersect(Math::CRay3 aRay) const
{
    RayTracingResult dReturnResult(Q3D_INFINITY);

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        RayTracingResult dNewResult = intersectRecurse(pComponent, aRay);

        if (dNewResult.m_dDistance < dReturnResult.m_dDistance)
        {
            dReturnResult = dNewResult;
        }
    }

    return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects the component specified by \a pComponent, or any of its children.
*/
RayTracingResult C3DScene::intersectComponentHierarchy(QSP<CComponent> pComponent, Math::CRay3 aRay) const
{
    RayTracingResult aResult = intersectRecurse(pComponent, aRay);
    return aResult;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult C3DScene::intersectRecurse(QSP<CComponent> pComponent, const Math::CRay3& aRay) const
{
    RayTracingResult dReturnResult = pComponent->intersect(aRay);

    foreach (QSP<CComponent> pChild, pComponent->getChildren())
    {
        RayTracingResult dChildResult = intersectRecurse(pChild, aRay);

        if (dChildResult.m_dDistance < dReturnResult.m_dDistance)
        {
            dReturnResult = dChildResult;
        }
    }

    return dReturnResult;
}

//-------------------------------------------------------------------------------------------------

void C3DScene::addSegment(Math::CVector3 vStart, Math::CVector3 vEnd)
{
    if (m_pSegments->vertices().count() > 400) return;

    if (m_bEditMode == false)
    {
        m_pSegments->vertices().append(CVertex(vStart - m_vWorldOrigin));
        m_pSegments->vertices().append(CVertex(vEnd - m_vWorldOrigin));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this scene to \a stream using the indentation value in \a iIdent.
*/
void C3DScene::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[C3DScene]"));
    dumpIdent(stream, iIdent, QString("CComponent::getNumComponents() : %1").arg(CComponent::getNumComponents()));

    foreach (QSP<CComponent> pComponent, m_vComponents)
    {
        dumpOpenBlock(stream, iIdent);
        pComponent->dump(stream, iIdent + 1);
        dumpCloseBlock(stream, iIdent);
    }
}
