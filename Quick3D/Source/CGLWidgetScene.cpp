
// qt-plus
#include "CLogger.h"

// Application
#include "CVector3.h"
#include "CGLWidgetScene.h"
#include "CRessourcesManager.h"
#include "CTreeGenerator.h"
#include "CBuildingGenerator.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define ATMOSPHERE_ALTITUDE     100000.0

//-------------------------------------------------------------------------------------------------

CGLWidgetScene::CGLWidgetScene(bool bForDisplay)
    : QGLWidget(QGLFormat (QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer))
    , C3DScene(bForDisplay)
{
    makeCurrentRenderingContext();

    if (m_bForDisplay)
    {
        m_pGLExtension = new CGLExtension();
        m_pRessourcesManager = new CRessourcesManager(this);
        m_pBuildingGenerator = new CBuildingGenerator(this);
        m_pTreeGenerator = new CTreeGenerator(this);
        m_vShaders = new CShaderCollection();

        CMaterial* pMaterial = new CMaterial(this);
        pMaterial->diffuse() = CVector4(1.0, 0.0, 0.0, 1.0);
        pMaterial->setLines(true);
        m_pSegments->setMaterial(QSP<CMaterial>(pMaterial));
        m_pSegments->setGLType(GL_LINES);
    }
}

//-------------------------------------------------------------------------------------------------

CGLWidgetScene::~CGLWidgetScene()
{
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::paintGL()
{
    if (m_bForDisplay)
    {
        m_tStatistics.reset();

        makeCurrentRenderingContext();

        //-------------------------------------------------------------------------------------------------
        // Clear frame buffer

        if (m_bDepthComputing)
        {
            glClearColor(1.0, 1.0, 1.0, 1.0);
        }
        else
        {
            glClearColor(m_tFog.color().X, m_tFog.color().Y, m_tFog.color().Z, 1.0);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------------------------------------------------------------------------------
        // Compute world origin

        if (m_pViewports.count() > 0 && m_pViewports[0]->camera())
        {
            m_vWorldOrigin = m_pViewports[0]->camera()->worldPosition();

            m_vWorldOrigin.X = m_vWorldOrigin.X - fmod(m_vWorldOrigin.X, 1000.0);
            m_vWorldOrigin.Y = m_vWorldOrigin.Y - fmod(m_vWorldOrigin.Y, 1000.0);
            m_vWorldOrigin.Z = m_vWorldOrigin.Z - fmod(m_vWorldOrigin.Z, 1000.0);
        }

        //-------------------------------------------------------------------------------------------------
        // Render cameras

        foreach (int iIndex, m_pViewports.keys())
        {
            if (m_pViewports[iIndex]->isEnabled() && m_pViewports[iIndex]->camera())
            {
                m_pViewports[iIndex]->camera()->render(this, m_pViewports[iIndex], m_bforceWideFOV, m_bforceSmallFOV, m_bForceIR, m_bOverlookScene);
            }
        }

        //-------------------------------------------------------------------------------------------------
        // Get the frame buffer

        bool bGrabFrame = false;

        foreach (int iIndex, m_pViewports.keys())
        {
            if (m_pViewports[iIndex]->streamView() || m_pViewports[iIndex]->needFrameBuffer())
            {
                bGrabFrame = true;
                break;
            }
        }

        if (bGrabFrame)
        {
            m_imgFrameBuffer = grabFrameBuffer();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::init(QVector<QSP<CComponent> > vComponents)
{
    LOG_METHOD_DEBUG("");

    if (m_bForDisplay)
    {
        makeCurrentRenderingContext();

        initShaders();
    }

    C3DScene::init(vComponents);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::initShaders()
{
    LOG_METHOD_DEBUG("");

    m_vShaders->clear();

    if (m_bForDisplay)
    {
        makeCurrentRenderingContext();

        QGLShaderProgram* pProgram;

        setlocale(LC_NUMERIC, "C");

        //-----------------------------------------------
        // Standard mesh

        pProgram = new QGLShaderProgram();

        pProgram->addShaderFromSourceCode(QGLShader::Vertex, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/VS_Standard.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Geometry, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/GS_Standard_Triangle.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Fragment, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/FS_Standard.c"));

        if (pProgram->link())
        {
            m_vShaders->addShader(SP_Standard_Mesh, pProgram);
        }
        else
        {
            delete pProgram;
        }

        //-----------------------------------------------
        // Standard billboard

        pProgram = new QGLShaderProgram();

        pProgram->addShaderFromSourceCode(QGLShader::Vertex, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/VS_Standard.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Geometry, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/GS_Standard_Billboard.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Fragment, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/FS_Standard.c"));

        if (pProgram->link())
        {
            m_vShaders->addShader(SP_Standard_Billboard, pProgram);
        }
        else
        {
            delete pProgram;
        }

        //-----------------------------------------------
        // Standard lines

        pProgram = new QGLShaderProgram();

        pProgram->addShaderFromSourceCode(QGLShader::Vertex, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/VS_Standard.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Geometry, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/GS_Standard_Line.c"));
        pProgram->addShaderFromSourceCode(QGLShader::Fragment, ressourcesManager()->getShaderByFilePathName(":/Resources/Shaders/FS_Special_Line.c"));

        if (pProgram->link())
        {
            m_vShaders->addShader(SP_Special_Lines, pProgram);
        }
        else
        {
            delete pProgram;
        }

        //-----------------------------------------------

        setlocale(LC_ALL, "");
    }

    C3DScene::initShaders();
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem)
{
    if (m_bForDisplay)
    {
        // Ici on transfère tous les paramètres de rendu au shader actif

        pProgram->setUniformValue("u_resolution", QVector2D(width(), height()));
        pProgram->setUniformValue("u_time", (GLfloat) m_dTime);
        pProgram->setUniformValue("u_shaderQuality", (GLfloat) m_dShaderQuality);
        pProgram->setUniformValue("u_rendering_shadows", (GLint) m_bRenderingShadows);
        pProgram->setUniformValue("u_normals_only", (GLint) m_bNormalsOnly);

        // Camera

        CVector3 vCamTruePos = pContext->camera()->worldPosition();
        CVector3 vCamPos = pContext->camera()->worldPosition() - m_vWorldOrigin;
        CVector3 vCamRot = pContext->camera()->worldRotation();
        CVector3 vWorldUp = pContext->camera()->worldPosition().normalized();

        QMatrix4x4 mCameraMatrix;
        mCameraMatrix.setToIdentity();
        mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.Y), QVector3D(0, 1, 0));
        mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.X), QVector3D(1, 0, 0));
        mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.Z), QVector3D(0, 0, 1));

        QVector3D vFront = (QVector3D(0.0, 0.0, 1.0) * mCameraMatrix) - (QVector3D(0.0, 0.0, 0.0) * mCameraMatrix);
        QVector3D vUp = (QVector3D(0.0, 1.0, 0.0) * mCameraMatrix) - (QVector3D(0.0, 0.0, 0.0) * mCameraMatrix);

        pProgram->setUniformValue("u_depth_computing", m_bDepthComputing ? 1 : 0);

        pProgram->setUniformValue("u_camera_true_position", QVector3D(vCamTruePos.X, vCamTruePos.Y, vCamTruePos.Z));
        pProgram->setUniformValue("u_camera_position", QVector3D(vCamPos.X, vCamPos.Y, vCamPos.Z));
        pProgram->setUniformValue("u_camera_direction", QVector3D(vFront.x(), vFront.y(), vFront.z()));
        pProgram->setUniformValue("u_camera_up", QVector3D(vUp.x(), vUp.y(), vUp.z()));
        pProgram->setUniformValue("u_world_origin", QVector3D(m_vWorldOrigin.X, m_vWorldOrigin.Y, m_vWorldOrigin.Z));
        pProgram->setUniformValue("u_world_up", QVector3D(vWorldUp.X, vWorldUp.Y, vWorldUp.Z));
        pProgram->setUniformValue("u_camera_altitude", (GLfloat) pContext->camera()->geoloc().Altitude);
        pProgram->setUniformValue("u_atmosphere_altitude", (GLfloat) ATMOSPHERE_ALTITUDE);

        // Lights

        pProgram->setUniformValue("u_global_ambient", QVector3D(0.05, 0.05, 0.15));
        pProgram->setUniformValue("u_shadow_enable", u_shadow_enable);

        pProgram->setUniformValue("u_num_lights", (GLint) iOpenGLLightIndex);
        pProgram->setUniformValueArray("u_light_is_sun", u_light_is_sun, MAX_GL_LIGHTS);
        pProgram->setUniformValueArray("u_light_position", u_light_position, MAX_GL_LIGHTS);
        pProgram->setUniformValueArray("u_light_screen_position", u_light_screen_position, MAX_GL_LIGHTS);
        pProgram->setUniformValueArray("u_light_direction", u_light_direction, MAX_GL_LIGHTS);
        pProgram->setUniformValueArray("u_light_color", u_light_color, MAX_GL_LIGHTS);
        pProgram->setUniformValueArray("u_light_distance_to_camera", u_light_distance_to_camera, MAX_GL_LIGHTS, 1);
        pProgram->setUniformValueArray("u_light_distance", u_light_distance, MAX_GL_LIGHTS, 1);
        pProgram->setUniformValueArray("u_light_spot_angle", u_light_spot_angle, MAX_GL_LIGHTS, 1);
        pProgram->setUniformValueArray("u_light_occlusion", u_light_occlusion, MAX_GL_LIGHTS, 1);

        pProgram->setUniformValue("u_fog_enable", (GLint) m_tFog.enabled() ? 1 : 0);
        pProgram->setUniformValue("u_fog_distance", (GLfloat) m_tFog.distance());
        pProgram->setUniformValue("u_fog_color", QVector3D(m_tFog.color().X, m_tFog.color().Y, m_tFog.color().Z));
        pProgram->setUniformValue("u_sun_color", vSunColor);
    }

    C3DScene::setupEnvironment(pContext, pProgram, bBackgroundItem);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::setupLights(CRenderContext* pContext)
{
    if (m_bForDisplay)
    {
        m_tFog.color() = CVector3(0.2, 0.6, 1.0);

        double dNormalizedTime = fabs(((double) m_tTimeOfDay.secsTo(QTime(0, 0, 0)) /  86400.0));
        double dSunAngle = (dNormalizedTime * -360.0) + 180.0;

        CGeoloc gSunPosition(0.0, dSunAngle, 100000000.0);
        CVector3 vSunPosition = gSunPosition.toVector3();

        // Compute sun intensity
        m_dSunIntensity = vSunPosition.normalized().dot(pContext->camera()->worldPosition().normalized());
        m_dSunIntensity = (m_dSunIntensity + 1.0) * 0.5;

        if (pContext->camera()->geoloc().Altitude < 0.0)
        {
            double dSeaFactor = 1.0 - (fabs(pContext->camera()->geoloc().Altitude) / 1000.0);
            if (dSeaFactor < 0.1) dSeaFactor = 0.1;
            m_dSunIntensity *= dSeaFactor;
        }

        double dAtmosphereFactor = (ATMOSPHERE_ALTITUDE - pContext->camera()->geoloc().Altitude) / ATMOSPHERE_ALTITUDE;
        if (dAtmosphereFactor < 0.0) dAtmosphereFactor = 0.0;
        if (dAtmosphereFactor > 1.0) dAtmosphereFactor = 1.0;

        // Compute fog distance
        m_tFog.distance() = (1.0 - m_tFog.level()) * pContext->camera()->maxDistance();

        // Underwater fog
        if (pContext->camera()->geoloc().Altitude < 0.0)
        {
            m_tFog.distance() = 1000.0;
        }

        m_dSunIntensity = Math::Angles::_max(m_dSunIntensity, 1.0 - dAtmosphereFactor);

        QVector<QSP<CLight> > vLights = lights();
        QVector<QSP<CLight> > vSuns = lightsByTag("SUN");

        if (vSuns.count() > 0)
        {
            vSuns[0]->setPosition(vSunPosition);

            if (pContext->camera()->geoloc().Altitude >= 0.0)
            {
                vSuns[0]->material()->diffuse() = m_iSunColor.getValue(m_dSunIntensity);
            }
            else
            {
                vSuns[0]->material()->diffuse() = CVector4(0.00, 0.25, 0.50, 1.00);
            }
        }

        // Multiply fog color with sun intensity
        m_tFog.color() = m_tFog.color() * m_dSunIntensity;

        if (vSuns.count() > 0)
        {
            CVector4 vColor = vSuns[0]->material()->diffuse();
            vSunColor = QVector3D(vColor.X, vColor.Y, vColor.Z);
        }
        else
        {
            vSunColor = QVector3D();
        }

        iOpenGLLightIndex = 0;

        for (int iLightIndex = 0; iLightIndex < vLights.count() && iOpenGLLightIndex < MAX_GL_LIGHTS; iLightIndex++)
        {
            CVector4 vColor = vLights[iLightIndex]->material()->diffuse();

            if (vColor.X != 0.0 || vColor.Y != 0.0 || vColor.Z != 0.0)
            {
                CVector3 vLightPosition = vLights[iLightIndex]->worldPosition();
                CVector3 vWorldPosition = vLightPosition - m_vWorldOrigin;
                CVector3 vWorldDirection = vLights[iLightIndex]->worldDirection();

                double dLightDistance = (vLightPosition - pContext->camera()->worldPosition()).magnitude();

                if (vLights[iLightIndex]->tag() == "SUN" || dLightDistance < 2000.0)
                {
                    QVector4D vRelativePosition(vWorldPosition.X, vWorldPosition.Y, vWorldPosition.Z, 1.0);
                    vRelativePosition = pContext->cameraMatrix() * vRelativePosition;
                    QVector4D vProjectedPosition = pContext->cameraProjectionMatrix() * vRelativePosition;
                    if (vProjectedPosition.w() != 0.0)
                    {
                        vProjectedPosition.setX(vProjectedPosition.x() / (vProjectedPosition.w() * 2.0) + 0.5);
                        vProjectedPosition.setY(vProjectedPosition.y() / (vProjectedPosition.w() * 2.0) + 0.5);
                        vProjectedPosition.setZ(vProjectedPosition.z() / (vProjectedPosition.w() * 2.0));
                    }
                    CVector3 vScreenPosition(vProjectedPosition.x(), vProjectedPosition.y(), vProjectedPosition.z());

                    if (vLights[iLightIndex]->tag() == "SUN")
                    {
                        vWorldDirection = CVector3();
                    }

                    u_light_is_sun[iOpenGLLightIndex]               = (GLint) (vLights[iLightIndex]->tag() == "SUN");
                    u_light_position[iOpenGLLightIndex]             = QVector3D(vWorldPosition.X, vWorldPosition.Y, vWorldPosition.Z);
                    u_light_screen_position[iOpenGLLightIndex]      = QVector3D(vScreenPosition.X, vScreenPosition.Y, vScreenPosition.Z);
                    u_light_direction[iOpenGLLightIndex]            = QVector3D(vWorldDirection.X, vWorldDirection.Y, vWorldDirection.Z);
                    u_light_color[iOpenGLLightIndex]                = QVector3D(vColor.X, vColor.Y, vColor.Z);
                    u_light_distance_to_camera[iOpenGLLightIndex]   = (GLfloat) vRelativePosition.length();
                    u_light_distance[iOpenGLLightIndex]             = (GLfloat) vLights[iLightIndex]->lightingDistance();
                    u_light_spot_angle[iOpenGLLightIndex]           = (GLfloat) Math::Angles::toRad(vLights[iLightIndex]->verticalFOV());
                    u_light_occlusion[iOpenGLLightIndex]            = (GLfloat) vLights[iLightIndex]->occlusion();

                    iOpenGLLightIndex++;
                }
            }
        }

        if (m_dShaderQuality >= 0.90)
        {
            u_shadow_enable = (GLint) (vSuns.count() > 0 && vSuns[0]->castShadows());
        }
        else
        {
            u_shadow_enable = 0;
        }

        if (vSuns.count() > 0 && vSuns[0]->castShadows())
        {
            vSuns[0]->material()->activateShadow(pContext);
        }

        // Compute light occlusions
        // computeLightsOcclusion(pContext);
    }

    C3DScene::setupLights(pContext);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::computeLightsOcclusion(CRenderContext* pContext)
{
    QVector<QSP<CLight> > vLights = lights();

    foreach (QSP<CLight> pLight, vLights)
    {
        CRay3 aRay;
        aRay.vOrigin = pLight->worldPosition();
        aRay.vNormal = (pContext->camera()->worldPosition() - aRay.vOrigin).normalized();

        RayTracingResult aResult = pContext->scene()->intersect(aRay);

        pLight->setOcclusion(aResult.m_dDistance < Q3D_INFINITY ? 1.0 : 0.0);
    }
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::makeCurrentRenderingContext()
{
    makeCurrent();
}
