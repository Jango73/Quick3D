
#pragma once

// Qt
#include <QGLWidget>
#include <QImage>
#include <QTime>

// qt-plus
#include "CInterpolator.h"
#include "CAverager.h"

// Application
#include "quick3d_global.h"
#include "C3DSceneStatistics.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "CDumpable.h"
#include "CGLExtension.h"
#include "CRessourcesManager.h"
#include "CBuildingGenerator.h"
#include "CTreeGenerator.h"
#include "CShaderCollection.h"
#include "CCamera.h"
#include "CLight.h"
#include "CFog.h"
#include "CRain.h"
#include "CRenderContext.h"
#include "CViewport.h"
#include "CMeshByMaterial.h"

//-------------------------------------------------------------------------------------------------

// #define DRAW_BY_MATERIAL

//-------------------------------------------------------------------------------------------------

class CView;
class CController;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT C3DScene : public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    C3DScene(bool bForDisplay = true);

    //! Destructor
    virtual ~C3DScene();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setWorldOrigin(Math::CVector3 value);

    //!
    void setController(CController* pController);

    //!
    void setDefaultController(CController* pController);

    //!
    void setShaderQuality(double value);

    //!
    void setTime(double value);

    //!
    void setTimeOfDay(QTime value);

    //!
    void setFogLevel(double value);

    //!
    void setWindLevel(double value);

    //!
    void setRainVisible(bool bOnOff);

    //!
    void forceWideFOV(bool value);

    //!
    void forceSmallFOV(bool value);

    //!
    void forceIR(bool value);

    //!
    void setStreamView(bool value) { m_bStreamView = value; }

    //!
    void setDepthComputing(bool value) { m_bDepthComputing = value; }

    //!
    void setRenderingShadows(bool value) { m_bRenderingShadows = value; }

    //!
    void setEditMode(bool value) { m_bEditMode = value; }

    //!
    void setDebugMode(bool value) { m_DebugMode = value; }

    //!
    void setBoundsOnly(bool value) { m_bBoundsOnly = value; }

    //!
    void setNormalsOnly(bool value) { m_bNormalsOnly = value; }

    //! Sets the overlook scene flag, for debug purposes
    void setOverlookScene(bool value) { m_bOverlookScene = value; }

    //!
    void setOverlookFOV(double value) { m_dOverlookFOV = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool forDisplay() const { return m_bForDisplay; }

    //! Returns the scene origin in the ECEF frame (Earth-centered Earth-Fixed)
    Math::CVector3 worldOrigin() const { return m_vWorldOrigin; }

    //! Returns a reference to the map of viewports
    QMap<int, CViewport*>& viewports() { return m_pViewports; }

    //! Returns a const reference to the map of viewports
    const QMap<int, CViewport*>& viewports() const { return m_pViewports; }

    //! Returns the shader quality setting
    double shaderQuality() { return m_dShaderQuality; }

    //! Returns a pointer to OpenGL extensions
    CGLExtension* glExtension() { return m_pGLExtension; }

    //! Returns the resource manager
    CRessourcesManager* ressourcesManager() { return m_pRessourcesManager; }

    //! Returns a generator for buildings
    CBuildingGenerator* buildingGenerator() { return m_pBuildingGenerator; }

    //! Returns a generator for trees
    CTreeGenerator* treeGenerator() { return m_pTreeGenerator; }

    //! Returns a collection of shaders for the scene
    CShaderCollection* shaders() { return m_vShaders; }

    //! Returns the current controller of the scene
    CController* controller() { return m_pController; }

    //! Returns a vector of all lights.
    QVector<QSP<CLight> > lights();

    //! Returns a reference to the vector of components.
    QVector<QSP<CComponent> >& components() { return m_vComponents; }

    //! Returns a const reference to the vector of components.
    const QVector<QSP<CComponent> >& components() const { return m_vComponents; }

    //! Returns a vector of components whose tag matches \a sTag.
    QVector<QSP<CComponent> > componentsByTag(const QString& sTag);

    //! Returns a vector of lights whose tag matches \a sTag.
    QVector<QSP<CLight> > lightsByTag(const QString& sTag);

    //!
    CFog& fog() { return m_tFog; }

    //!
    double time() const { return m_dTime; }

    //!
    double windLevel() const { return m_dWindLevel; }

    //!
    bool isRenderingShadows() const { return m_bRenderingShadows; }

    //!
    bool streamView() const { return m_bStreamView; }

    //!
    bool depthComputing() const { return m_bDepthComputing; }

    //!
    const QImage& frameBuffer() const { return m_imgFrameBuffer; }

    //!
    bool editMode() const { return m_bEditMode; }

    //!
    bool debugMode() const { return m_DebugMode; }

    //!
    bool frustumCheck() const { return m_bFrustumCheck; }

    //!
    bool boundsOnly() const { return m_bBoundsOnly; }

    //!
    bool normalsOnly() const { return m_bNormalsOnly; }

    //! Returns the overlook scene flag, for debug purposes
    bool overlookScene() const { return m_bOverlookScene; }

    //!
    double overlookFOV() const { return m_dOverlookFOV; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Destroys all components and viewports
    void clear();

    //! Destroys all components
    void clearComponents();

    //! Destroys all viewports
    void clearViewports();

    //! Sets up the scene
    virtual void init(QVector<QSP<CComponent> > vComponents);

    //! Sets up shaders
    virtual void initShaders();

    //! Sets up the environment
    virtual void setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem);

    //! Sets up the ligths
    virtual void setupLights(CRenderContext* pContext);

    //!
    virtual void makeCurrentRenderingContext();

    //!
    void autoResolveHeightFields();

    //! Updates the scene using elapsed time in \a dDeltaTimeS.
    void updateScene(double dDeltaTime);

    //! Paints all components, using \a pContext.
    void paintComponents(CRenderContext* pContext);

    //! Paints all components that cast shadows, using \a pContext.
    void paintShadowCastingComponents(CRenderContext* pContext);

    //! Adds \a pComponent to the scene
    void addComponent(QSP<CComponent> pComponent);

    //! Deletes components whose tag match \a sTag
    void deleteComponentsByTag(const QString& sTag);

    //! Ray intersection
    virtual Math::RayTracingResult intersect(Math::CRay3 rRay) const;

    //! Ray intersection
    virtual Math::RayTracingResult intersectComponentHierarchy(QSP<CComponent> pComponent, Math::CRay3 aRay) const;

    //! Ray intersection
    Math::RayTracingResult intersectRecurse(QSP<CComponent> pComponent, const Math::CRay3& aRay) const;

    //!
    void addSegment(Math::CVector3 vStart, Math::CVector3 vEnd);

    //!
    QString debugInfo();

    //! Dumps contents in a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    static void getLightsRecurse(QVector<QSP<CLight> >& vLights, QSP<CComponent> pComponent);

    //!
    static void getLightsByTagRecurse(QVector<QSP<CLight> >& vLights, const QString &sTag, QSP<CComponent> pComponent);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

public:

    C3DSceneStatistics                      m_tStatistics;
    CRain*                                  m_pRain;

protected:

    QMap<int, CViewport*>                   m_pViewports;
    CGLExtension*                           m_pGLExtension;
    CRessourcesManager*                     m_pRessourcesManager;
    CBuildingGenerator*                     m_pBuildingGenerator;
    CTreeGenerator*                         m_pTreeGenerator;
    CShaderCollection*                      m_vShaders;
    CController*                            m_pController;
    CController*                            m_pDefaultController;
    Math::CVector3                          m_vWorldOrigin;
    QTime                                   m_tTimeOfDay;
    CAverager<double>                       m_FPS;
    CFog                                    m_tFog;
    CInterpolator<Math::CVector4>           m_iSunColor;
    bool                                    m_bForDisplay;
    bool                                    m_bFrustumCheck;
    bool                                    m_bEditMode;
    bool                                    m_DebugMode;
    bool                                    m_bBoundsOnly;
    bool                                    m_bNormalsOnly;
    bool                                    m_bOverlookScene;
    double                                  m_dShaderQuality;
    double                                  m_dWindLevel;
    bool                                    m_bRenderingShadows;
    bool                                    m_bforceWideFOV;
    bool                                    m_bforceSmallFOV;
    bool                                    m_bForceIR;
    bool                                    m_bStreamView;
    bool                                    m_bDepthComputing;
    double                                  m_dTime;
    double                                  m_dSunIntensity;
    double                                  m_dOverlookFOV;
    QImage                                  m_imgFrameBuffer;

    // Shared data

    QVector<QSP<CComponent> >               m_vComponents;
    QSP<CMeshGeometry>                      m_pSegments;
};
