
// Qt
#include <QColor>

// qt-plus
#include "CLogger.h"
#include "CTimeSampler.h"

// Application
#include "CCamera.h"
#include "C3DScene.h"
#include "CGLWidgetScene.h"
#include "CRenderContext.h"
#include "CImageUtilities.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define DEFAULT_FOV	80.0
#define SMALL_FOV	10.0

//-------------------------------------------------------------------------------------------------

inline CVector2 degreesToPixels(double dCameraFOVW, double dCameraFOVH, CVector2 vAnglesDegrees)
{
    double dLW = 0.5 / tan(Math::Angles::toRad(dCameraFOVW / 2.0));
    double dLH = 0.5 / tan(Math::Angles::toRad(dCameraFOVH / 2.0));

    double dPixX = (dLW * tan(Math::Angles::toRad(vAnglesDegrees.X)));
    double dPixY = (dLH * tan(Math::Angles::toRad(vAnglesDegrees.Y)));

    return CVector2(0.5 + dPixX, 0.5 + dPixY);
}

//-------------------------------------------------------------------------------------------------

CComponent* CCamera::instantiator(C3DScene* pScene)
{
    return new CCamera(pScene);
}

//-------------------------------------------------------------------------------------------------

CCamera::CCamera(C3DScene* pScene)
    : CPhysicalComponent(pScene)
    , m_dFOV(DEFAULT_FOV)
    , m_dFocus(0.5)
    , m_dGain(0.5)
    , m_dMinDistance(0.0)
    , m_dMaxDistance(0.0)
{
    setName("Camera");
    setRaytracable(false);
}

//-------------------------------------------------------------------------------------------------

CCamera::CCamera(const CCamera& target)
    : CPhysicalComponent(target)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CCamera::~CCamera()
{
}

//-------------------------------------------------------------------------------------------------

CCamera& CCamera::operator = (const CCamera& target)
{
    CPhysicalComponent::operator =(target);

    m_dFOV              = target.m_dFOV;
    m_dFocus            = target.m_dFocus;
    m_dGain             = target.m_dGain;
    m_dMinDistance      = target.m_dMinDistance;
    m_dMaxDistance      = target.m_dMaxDistance;
    m_pFrustumPlanes    = target.m_pFrustumPlanes;

    return *this;
}

//-------------------------------------------------------------------------------------------------

void CCamera::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CPhysicalComponent::loadParameters(sBaseFile, xComponent);

    CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

    if (xGeneralNode.isEmpty() == false)
    {
        if (xGeneralNode.attributes()[ParamName_FOV].isEmpty() == false)
        {
            m_dFOV = xGeneralNode.attributes()[ParamName_FOV].toDouble();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCamera::render(C3DScene* pScene, CViewport* pViewport, bool bForceWideFOV, bool bForceSmallFOV, bool bForceIR, bool bOverlook)
{
    //-------------------------------------------------------------------------------------------------
    // OpenGL settings

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    //-------------------------------------------------------------------------------------------------

    QVector<QSP<CLight> > vSuns = pScene->lightsByTag("SUN");

    QMatrix4x4 mShadowProjectionMatrix;
    QMatrix4x4 mShadowMatrix;

    if (pScene->shaderQuality() >= 0.90)
    {
        pScene->setRenderingShadows(true);

        if (vSuns.count() > 0 && vSuns[0]->castShadows())
        {
            QSP<CLight> pLight = vSuns[0];

            pLight->setMinDistance(1.0);
            pLight->setMaxDistance(2000.0);

            pLight->saveTransform();

            CVector3 vDirection = (pLight->worldPosition() - this->worldPosition()).normalized();
            pLight->setPosition(this->worldPosition() + vDirection * 1000.0);
            pLight->lookAt(this);

            CVector3 vCamPos = pLight->worldPosition();
            CVector3 vCamRot = pLight->worldRotation();

            mShadowMatrix = CCamera::getQtCameraMatrix(vCamPos - pScene->worldOrigin(), vCamRot);
            mShadowProjectionMatrix = CCamera::getQtProjectionMatrix(pLight->verticalFOV(), 1.0, pLight->minDistance(), pLight->maxDistance());

            CMatrix4 mInternalCameraMatrix = CCamera::getInternalCameraMatrix(vCamPos, vCamRot);
            CMatrix4 mInternalProjectionMatrix = CCamera::getInternalProjectionMatrix(pLight->verticalFOV(), 1.0, pLight->minDistance(), pLight->maxDistance());

            CRenderContext Context(
                        mShadowProjectionMatrix, mShadowMatrix,
                        mShadowProjectionMatrix, mShadowMatrix,
                        mInternalCameraMatrix,
                        mInternalProjectionMatrix,
                        pScene,
                        pLight.data()
                        );

            pLight->computeFrustum(Math::Angles::toRad(pLight->verticalFOV()), 1.0, pLight->minDistance(), pLight->maxDistance());

            //-------------------------------------------------------------------------------------------------
            // Render objects

            pLight->material()->enableFrameBuffer();

            glViewport(0, 0, 2048, 2048);

            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClearStencil(0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            pScene->paintShadowCastingComponents(&Context);

            pLight->material()->disableFrameBuffer();

            //-------------------------------------------------------------------------------------------------

            pLight->loadTransform();

            //-------------------------------------------------------------------------------------------------
            // Get statistics

            pScene->m_tStatistics.m_iNumMeshesDrawn += Context.tStatistics.m_iNumMeshesDrawn;
            pScene->m_tStatistics.m_iNumPolysDrawn += Context.tStatistics.m_iNumPolysDrawn;
            pScene->m_tStatistics.m_iNumChunksDrawn += Context.tStatistics.m_iNumChunksDrawn;
            pScene->m_tStatistics.m_iNumFrustumTests += Context.tStatistics.m_iNumFrustumTests;
            pScene->m_tStatistics.m_iNumRayIntersectionTests += Context.tStatistics.m_iNumRayIntersectionTests;
        }

        pScene->setRenderingShadows(false);
    }

    //-------------------------------------------------------------------------------------------------
    // Viewport

    int iWidth =  pViewport->size().X;
    int iHeight = pViewport->size().Y;

    if (iHeight == 0) return;

    glViewport((int) pViewport->position().X, (int) pViewport->position().Y, iWidth, iHeight);

    //-------------------------------------------------------------------------------------------------
    // Rendering context and transform matrices

    if (pScene->shaderQuality() >= 0.90)
    {
        if (vSuns.count() > 0 && vSuns[0]->castShadows())
        {
            QSP<CLight> pLight = vSuns[0];

            pLight->saveTransform();

            CVector3 vDirection = (pLight->worldPosition() - this->worldPosition()).normalized();
            pLight->setPosition(this->worldPosition() + vDirection * 1000.0);
            pLight->lookAt(this);

            CVector3 vLitePos = pLight->worldPosition();
            CVector3 vLiteRot = pLight->worldRotation();

            double dMinDistance = 1.0;
            double dMaxDistance = 2000.0;

            mShadowMatrix = CCamera::getQtCameraMatrix(vLitePos - pScene->worldOrigin(), vLiteRot);
            mShadowProjectionMatrix = CCamera::getQtProjectionMatrix(pLight->verticalFOV(), 1.0, dMinDistance, dMaxDistance);

            pLight->loadTransform();
        }
    }

    double dFOV = m_dFOV;
    if (bForceWideFOV) dFOV = DEFAULT_FOV;
    if (bForceSmallFOV) dFOV = SMALL_FOV;

    double dMinDistance = geoloc().Altitude / 8.0;
    if (geoloc().Altitude < 20000.0) dMinDistance = 0.1;
    if (dMinDistance < 0.1) dMinDistance = 0.1;

    double dMaxDistance = geoloc().Altitude * 40.0;
    if (dMaxDistance < 100000.0) dMaxDistance = 100000.0;

    m_dMinDistance = dMinDistance;
    m_dMaxDistance = dMaxDistance;

    // For tests
    if (bOverlook)
    {
        m_dMaxDistance = geoloc().getPlanetRadius() * 2.0;
        m_dMinDistance = dMaxDistance / 100.0;
    }

    CVector3 vCamPos = worldPosition();
    CVector3 vCamRot = worldRotation();

    QMatrix4x4 mCameraMatrix = CCamera::getQtCameraMatrix(vCamPos - pScene->worldOrigin(), vCamRot);
    QMatrix4x4 mCameraProjection = CCamera::getQtProjectionMatrix(
                dFOV,
                (double) iWidth / (double) iHeight,
                m_dMinDistance,
                m_dMaxDistance
                );

    // For tests
    if (bOverlook)
    {
        mCameraMatrix = CCamera::getQtCameraMatrix(
                    CGeoloc(
                        geoloc().Latitude,
                        geoloc().Longitude,
                        geoloc().getPlanetRadius() * 0.5
                        ).toVector3() - pScene->worldOrigin(),
                    CAxis(CVector3(Math::Pi * 0.5, 0.0, 0.0)).transferTo(geoloc().getNOLLAxis()).eulerAngles()
                    );

        mCameraProjection = CCamera::getQtProjectionMatrix(
                    pScene->overlookFOV(),
                    (double) iWidth / (double) iHeight,
                    m_dMinDistance,
                    m_dMaxDistance
                    );
    }

    CMatrix4 m_mInternalCameraMatrix = CCamera::getInternalCameraMatrix(vCamPos, vCamRot);
    CMatrix4 m_mInternalProjectionMatrix = CMatrix4::fromQtMatrix(mCameraProjection);

    CRenderContext Context(
                mCameraProjection, mCameraMatrix,
                mShadowProjectionMatrix, mShadowMatrix,
                m_mInternalCameraMatrix,
                m_mInternalProjectionMatrix,
                pScene,
                this
                );

    computeFrustum(Math::Angles::toRad(dFOV), (double) iWidth / (double) iHeight, m_dMinDistance, m_dMaxDistance);

    //-------------------------------------------------------------------------------------------------
    // Update objects

    foreach(QSP<CComponent> pComponent, pScene->components())
    {
        if (pComponent->isVisible())
        {
            pComponent->updateContext(&Context);
            pComponent->updateItems(pScene);
        }
    }

    //-------------------------------------------------------------------------------------------------

    if (bForceIR)
    {
        Context.bUseIR = true;
    }

    //-------------------------------------------------------------------------------------------------
    // OpenGL settings

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //-------------------------------------------------------------------------------------------------
    // Lights

    pScene->setupLights(&Context);

    //-------------------------------------------------------------------------------------------------
    // Render objects

    pScene->paintComponents(&Context);

    //-------------------------------------------------------------------------------------------------
    // OpenGL settings

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    //-------------------------------------------------------------------------------------------------
    // Get statistics

    pScene->m_tStatistics.m_iNumMeshesDrawn += Context.tStatistics.m_iNumMeshesDrawn;
    pScene->m_tStatistics.m_iNumPolysDrawn += Context.tStatistics.m_iNumPolysDrawn;
    pScene->m_tStatistics.m_iNumChunksDrawn += Context.tStatistics.m_iNumChunksDrawn;
    pScene->m_tStatistics.m_iNumFrustumTests += Context.tStatistics.m_iNumFrustumTests;
    pScene->m_tStatistics.m_iNumRayIntersectionTests += Context.tStatistics.m_iNumRayIntersectionTests;
}

//-------------------------------------------------------------------------------------------------

void CCamera::renderDepth_RayTraced
(
        C3DScene* pScene,
        double dMaxDistance,
        CPanoramicMatrixParams& tParams,
        IProgressListener* pProgressListener
        )
{
    pScene->setWorldOrigin(worldPosition());

    CGeoloc gReference = geoloc();

    // Create polygons in local 3D space and viewed from top (X et Z)
    for (QVector<CGeoZone>::iterator gZone = tParams.m_vZones.begin(); gZone != tParams.m_vZones.end(); gZone++)
    {
        (*gZone).getLocalPoints().clear();

        foreach (const CGeoloc& gPoint, (*gZone).getPoints())
        {
            CVector3 vPoint = gPoint.toVector3(gReference);
            (*gZone).getLocalPoints().append(CVector2(vPoint.X, vPoint.Z));
        }
    }

    for (int iTilt = 0; iTilt < tParams.m_sResolution.height(); iTilt++)
    {
        if (pProgressListener != nullptr)
        {
            double dPercent = (((double) (iTilt * tParams.m_sResolution.width())) / (double) (tParams.m_sResolution.width() * tParams.m_sResolution.height())) * 100.0;
            pProgressListener->notifyProgress("", dPercent);
        }

        for (int iPan = 0; iPan < tParams.m_sResolution.width(); iPan++)
        {
            double dPanNormalized = (double) iPan / (double) tParams.m_sResolution.width();
            double dTiltNormalized = (double) iTilt / (double) tParams.m_sResolution.height();

            double dCurrentPan = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.Y + (dPanNormalized * (tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y)));
            double dCurrentTilt = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.X + (dTiltNormalized * (tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X)));

            // Create the ray for the current pixel
            CRay3 rCameraRay;

            // Ray points to +Z
            rCameraRay.vOrigin = CVector3(0.0, 0.0, 0.0);
            rCameraRay.vNormal = CVector3(0.0, 0.0, 1.0);

            // Apply current angles to ray
            rCameraRay = CMatrix4().makeRotation(CVector3(dCurrentTilt, 0.0, 0.0)) * rCameraRay;
            rCameraRay = CMatrix4().makeRotation(CVector3(0.0, dCurrentPan, 0.0)) * rCameraRay;

            // Apply camera transform to ray
            rCameraRay = worldTransform() * rCameraRay;

            // Initialize tracing distance
            RayTracingResult dResult(Q3D_INFINITY);

            foreach (QSP<CComponent> pComponent, pScene->components())
            {
                // Est-ce que le composant doit être pris en compte par le ray-tracing?
                if (pComponent->isVisible() && pComponent->isRaytracable())
                {
                    RayTracingResult dNewResult = pComponent->intersect(rCameraRay);

                    // On garde la distance remontée si elle est inférieure à elle déjà calculée
                    if (dNewResult.m_dDistance < dResult.m_dDistance)
                    {
                        dResult = dNewResult;
                    }
                }
            }

            CGeoZone::EGeoZoneFlag eDetectionFlag = CGeoZone::gzfUnknown;
            int iDotRayNormal = 0;

            if (dResult.m_dDistance >= 0.0 && dResult.m_dDistance < Q3D_INFINITY && dResult.m_dDistance < dMaxDistance)
            {
                CVector3 vIntersectionPoint3D = rCameraRay.vOrigin + rCameraRay.vNormal * dResult.m_dDistance;
                CGeoloc gIntersectionPoint(vIntersectionPoint3D);
                CVector3 vLocalIntersectionPoint3D = gIntersectionPoint.toVector3(gReference);

                // Get the current flag for detection zones
                eDetectionFlag = categorizePointFromZones(
                            tParams.m_vZones,
                            CVector2(vLocalIntersectionPoint3D.X, vLocalIntersectionPoint3D.Z)
                            );

                iDotRayNormal = (int) (rCameraRay.vNormal.dot(dResult.m_vNormal) * -255.0);
                iDotRayNormal = Math::Angles::clipInt(iDotRayNormal, 0, 255);
            }
            else
            {
                dResult.m_dDistance = -1.0;
            }

            // Fill matrices
            tParams.m_vDetection.append((char) eDetectionFlag);
            tParams.m_vEdges.append((char) iDotRayNormal);
            tParams.m_vDepth.append(dResult.m_dDistance);
        }
    }
}

//-------------------------------------------------------------------------------------------------

QImage getFrameBuffer(CGLWidgetScene* pScene, QSize sSize)
{
    QImage imgFrame(sSize, QImage::Format_RGB888);

    glReadPixels(0, 0, sSize.width(), sSize.height(), GL_RGB, GL_UNSIGNED_BYTE, imgFrame.bits());

    return imgFrame.mirrored(false, true);
}

//-------------------------------------------------------------------------------------------------

#define RENDER_MAP_SIZE_WIDTH_MIN		32
#define RENDER_MAP_SIZE_WIDTH_MAX		512

// Les deux chiffres suivants ne doivent pas être diminués sous peine d'avoir une matrice déformée
#define RENDER_SUBDIVISIONS_PAN			32
#define RENDER_SUBDIVISIONS_TILT		16

void CCamera::renderDepth_CubeMapped
(
        C3DScene* pScene,
        double dMaxDistance,
        CPanoramicMatrixParams& tParams,
        IProgressListener* pProgressListener
        )
{
    CGLWidgetScene* pGLWidgetScene = dynamic_cast<CGLWidgetScene*>(pScene);

    if (pGLWidgetScene != nullptr)
    {
        START_SAMPLE("CCamera::renderDepth_CubeMapped:prepare");

        CGeoloc gReference = geoloc();

        // Création des polygones de zone dans le repère 3D local vu de dessus (X et Z)
        for (QVector<CGeoZone>::iterator gZone = tParams.m_vZones.begin(); gZone != tParams.m_vZones.end(); gZone++)
        {
            // gZone.getLocalPoints().clear();
            (*gZone).getLocalPoints().clear();

            foreach (const CGeoloc& gPoint, (*gZone).getPoints())
            {
                CVector3 vPoint = gPoint.toVector3(gReference);
                (*gZone).getLocalPoints().append(CVector2(vPoint.X, vPoint.Z));
            }
        }

        // Calcul de la taille d'un mini-rendu
        // A revoir car ce calcul suppose que la matrice demandée fait 360°x180°

        /*
        int iRenderMapSize = RENDER_MAP_SIZE_WIDTH_MIN;

        while (iRenderMapSize < RENDER_MAP_SIZE_WIDTH_MAX)
        {
            if (RENDER_SUBDIVISIONS_PAN * iRenderMapSize >= tParams.m_sResolution.width() &&
                RENDER_SUBDIVISIONS_TILT * iRenderMapSize >= tParams.m_sResolution.height())
            {
                break;
            }

            iRenderMapSize *= 2;
        }
        */

        int iRenderMapSize = 265;	// Puissance de deux

        LOG_METHOD_DEBUG(
                    QString("Rendering %1 x %2 image tiles of %3 pixels resolution")
                    .arg(RENDER_SUBDIVISIONS_PAN)
                    .arg(RENDER_SUBDIVISIONS_TILT)
                    .arg(iRenderMapSize)
                    );

        // Calcul des FOVs
        double dFOV_Pan = 360.0 / (double) RENDER_SUBDIVISIONS_PAN;
        double dFOV_Tilt = 180.0 / (double) RENDER_SUBDIVISIONS_TILT;
        double dFOV_Ratio = dFOV_Tilt / dFOV_Pan;

        // Définition de la taille du viewport
        QSize sMapSize(iRenderMapSize, (int) ((double) iRenderMapSize * dFOV_Ratio));

        // Vecteur contenant la taille d'une image du cube-map
        CVector2 vMapSize((double) sMapSize.width(), (double) sMapSize.height());

        // Sauvegarde des paramètres de la caméra
        QRect rOldGeometry = pGLWidgetScene->geometry();
        CViewport* pOldViewport = pGLWidgetScene->viewports()[0];
        CViewport* pNewViewport = new CViewport(pGLWidgetScene);
        CVector3 vCameraInitialRotation = rotation();
        CVector3 vCameraAttitude = tParams.m_vAttitude.degreesToRadians() * -1.0;
        CVector3 vCameraHeading(0.0, Math::Angles::toRad(tParams.m_dCameraTrueHeadingDegrees), 0.0);
        double dOldFOV = m_dFOV;

        // Création du viewport pour le rendu de la scène
        pGLWidgetScene->setGeometry(0, 0, sMapSize.width(), sMapSize.height());
        pGLWidgetScene->viewports()[0] = pNewViewport;
        pGLWidgetScene->viewports()[0]->setSize(vMapSize);
        pGLWidgetScene->viewports()[0]->setCamera(QSP<CCamera>(this));
        pGLWidgetScene->viewports()[0]->setEnabled(true);
        pGLWidgetScene->setDepthComputing(true);
        pGLWidgetScene->setShaderQuality(0.0);
        pGLWidgetScene->updateScene(1.0);

        // Création du cube-map de référence
        QImage imgImage(QSize(sMapSize.width() * RENDER_SUBDIVISIONS_PAN, sMapSize.height() * RENDER_SUBDIVISIONS_TILT), QImage::Format_RGB888);

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:prepare");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:render");

        // Réglage du FOV caméra horizontal (le FOV vertical est fonction de la taille du viewport)
        m_dFOV = dFOV_Tilt;

        double dHalfFOV_Pan = dFOV_Pan * 0.5;
        double dHalfFOV_Tilt = dFOV_Tilt * 0.5;

        // Rendu de chaque portion d'image
        for (int iTiltIndex = 0; iTiltIndex < RENDER_SUBDIVISIONS_TILT; iTiltIndex++)
        {
            for (int iPanIndex = 0; iPanIndex < RENDER_SUBDIVISIONS_PAN; iPanIndex++)
            {
                double dCurrentPan = (360.0 / RENDER_SUBDIVISIONS_PAN) * (double) iPanIndex;
                double dCurrentTilt = (180.0 / RENDER_SUBDIVISIONS_TILT) * (double) iTiltIndex;

                dCurrentPan -= 180.0;
                dCurrentTilt -= 90.0;

                dCurrentPan += dHalfFOV_Pan;
                dCurrentTilt += dHalfFOV_Tilt;

                // Récupération de la rotation de l'image courante
                CVector3 vCurrentRotationRadians = CVector3(Math::Angles::toRad(dCurrentTilt), Math::Angles::toRad(dCurrentPan), 0.0);

                // Calcul des angles finaux de la caméra via un axe
                Math::CAxis anAxis;
                anAxis = anAxis.rotate(vCurrentRotationRadians);
                anAxis = anAxis.rotate(CVector3(0.0, vCameraAttitude.Y, 0.0));
                anAxis = anAxis.rotate(CVector3(vCameraAttitude.X, 0.0, 0.0));
                anAxis = anAxis.rotate(CVector3(0.0, 0.0, vCameraAttitude.Z));
                anAxis = anAxis.rotate(vCameraHeading);

                // Application des angles de l'axe à la caméra
                setRotation(anAxis.eulerAngles());

                // Rendu OpenGL
                pGLWidgetScene->paintGL();

                // Copie de l'image rendue dans l'image complète
                CImageUtilities::blitWhole(
                            imgImage,
                            QPoint(sMapSize.width() * iPanIndex, sMapSize.height() * iTiltIndex),
                            getFrameBuffer(pGLWidgetScene, sMapSize)
                            );
            }
        }

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:render");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:postprocess");

        // Pour tests : sauvegarde de l'image finale telle que rendue par OpenGL
        // imgImage.save("d:\\Pano.png");

        // Récupération des distances
        for (int iTilt = 0; iTilt < tParams.m_sResolution.height(); iTilt++)
        {
            for (int iPan = 0; iPan < tParams.m_sResolution.width(); iPan++)
            {
                // Calcul du pan et tilt courant dans l'espace image

                double dPanNormalized = (double) iPan / (double) tParams.m_sResolution.width();
                double dTiltNormalized = (double) iTilt / (double) tParams.m_sResolution.height();

                double dCurrentPan = dPanNormalized * (tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y);
                double dCurrentTilt = dTiltNormalized * (tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X);

                // Ajustements
                dCurrentPan = dCurrentPan + tParams.m_vStartPanTiltDegrees.Y;
                dCurrentTilt = dCurrentTilt + tParams.m_vStartPanTiltDegrees.X;

                dCurrentPan += 180.0;
                dCurrentTilt += 90.0;

                dCurrentPan = Math::Angles::clipAngleDegree(dCurrentPan);
                dCurrentTilt = Math::Angles::clipAngleDegree(dCurrentTilt);

                dCurrentPan = (dCurrentPan / 360.0) * imgImage.width();
                dCurrentTilt = (dCurrentTilt / 180.0) * imgImage.height();

                CVector2 vCoords(dCurrentPan, dCurrentTilt);

                // Initialisation des données de sortie
                double dDistance = -1.0;
                CGeoZone::EGeoZoneFlag eDetectionFlag = CGeoZone::gzfUnknown;
                int iDotRayNormal = -127;

                if (
                        vCoords.X >= 0.0 && vCoords.X < (double) imgImage.width() &&
                        vCoords.Y >= 0.0 && vCoords.Y < (double) imgImage.height()
                        )
                {
                    // Récupération du pixel, contient la distance et l'angle d'incidence
                    QRgb rgbPixel = imgImage.pixel((int) vCoords.X, (int) vCoords.Y);

                    // 0xFF = infini
                    if ((rgbPixel & 0xFF) == 0xFF)
                    {
                        dDistance = -1.0;
                    }
                    else
                    {
                        // Récupération de la distance
                        dDistance = ((double) ((rgbPixel >> 16) & 0xFF) / 255.0) * 10000.0;

                        // Produit scalaire entre vecteur "vue" et vecteur normal du terrain
                        // Autrement dit, angle d'incidence du terrain
                        iDotRayNormal = (rgbPixel >> 8) & 0xFF;

                        // Ici on travaille dans le repère NOLL (North-oriented local-level)

                        // Calcul du pan et tilt courant dans l'espace caméra
                        dCurrentPan = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.Y + (dPanNormalized * (tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y)));
                        dCurrentTilt = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.X + (dTiltNormalized * (tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X)));

                        // Création du rayon correspondant au point actuel
                        CRay3 rCameraRay;

                        // Le rayon pointe vers +Z
                        rCameraRay.vOrigin = CVector3(0.0, 0.0, 0.0);
                        rCameraRay.vNormal = CVector3(0.0, 0.0, 1.0);

                        // Application des angles courants au rayon
                        rCameraRay = CMatrix4().makeRotation(CVector3(dCurrentTilt, 0.0, 0.0)) * rCameraRay;
                        rCameraRay = CMatrix4().makeRotation(CVector3(0.0, dCurrentPan, 0.0)) * rCameraRay;

                        // Application du cap caméra au rayon
                        rCameraRay = CMatrix4().makeRotation(vCameraHeading) * rCameraRay;

                        // Calcul du point d'intersection
                        CVector3 vIntersectionPoint3D = rCameraRay.vOrigin + rCameraRay.vNormal * dDistance;

                        // On récupère le flag correspondant aux zones de détection
                        eDetectionFlag = categorizePointFromZones(
                                    tParams.m_vZones,
                                    CVector2(vIntersectionPoint3D.X, vIntersectionPoint3D.Z)
                                    );
                    }
                }

                // Remplissage des matrices
                tParams.m_vDetection.append((char) eDetectionFlag);
                tParams.m_vEdges.append((char) iDotRayNormal);
                tParams.m_vDepth.append(dDistance);
            }
        }

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:postprocess");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:cleanup");

        // Restauration des réglages de la scène
        pGLWidgetScene->setGeometry(rOldGeometry);
        pGLWidgetScene->viewports()[0] = pOldViewport;
        pGLWidgetScene->setDepthComputing(false);
        pGLWidgetScene->setShaderQuality(0.5);

        // Restauration des paramètres de la caméra
        setRotation(vCameraInitialRotation);
        m_dFOV = dOldFOV;

        // Destruction du viewport temporaire
        delete pNewViewport;

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:cleanup");
    }
}

//-------------------------------------------------------------------------------------------------

void CCamera::computeFrustum(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance)
{
    // Remise à zéro des plans du frustum (pyramide de visualisation)
    m_pFrustumPlanes.clear();

    double dHorizontalFOV = dVerticalFOV;

    // Création des matrices de rotation des plans
    CMatrix4 mRotateY1 = CMatrix4().makeRotation(CVector3(0.0, dHorizontalFOV *  1.0, 0.0));
    CMatrix4 mRotateY2 = CMatrix4().makeRotation(CVector3(0.0, dHorizontalFOV * -1.0, 0.0));
    CMatrix4 mRotateX1 = CMatrix4().makeRotation(CVector3(dVerticalFOV *  1.0, 0.0, 0.0));
    CMatrix4 mRotateX2 = CMatrix4().makeRotation(CVector3(dVerticalFOV * -1.0, 0.0, 0.0));

    // Création des vecteurs normaux des plans
    CVector3 v1(-1.0,  0.0,  0.0);
    CVector3 v2( 1.0,  0.0,  0.0);
    CVector3 v3( 0.0,  1.0,  0.0);
    CVector3 v4( 0.0, -1.0,  0.0);
    CVector3 v5( 0.0,  0.0,  1.0);
    CVector3 v6( 0.0,  0.0, -1.0);

    // Ajout de chaque plan
    m_pFrustumPlanes.append(CPlane3(0.0, mRotateY1 * v1));
    m_pFrustumPlanes.append(CPlane3(0.0, mRotateY2 * v2));
    m_pFrustumPlanes.append(CPlane3(0.0, mRotateX1 * v3));
    m_pFrustumPlanes.append(CPlane3(0.0, mRotateX2 * v4));
    m_pFrustumPlanes.append(CPlane3(dMinDistance, v5));
    m_pFrustumPlanes.append(CPlane3(dMaxDistance, v6));
}

//-------------------------------------------------------------------------------------------------

bool CCamera::contains(const CVector3& vPosition, double dRadius) const
{
    // Test du vecteur avec chaque plan du frustum
    foreach (CPlane3 plane, m_pFrustumPlanes)
    {
        double dDistance = plane.vNormal.dot(vPosition) + plane.dDistance;
        if (dDistance < -dRadius) return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

bool CCamera::contains(const CBoundingBox& bounds) const
{
    return contains(bounds.center(), bounds.radius());
}

//-------------------------------------------------------------------------------------------------

QMatrix4x4 CCamera::getQtProjectionMatrix(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance)
{
    // Création d'uine matrice de projection Qt
    QMatrix4x4 mMatrix;

    mMatrix.setToIdentity();
    mMatrix.perspective(dVerticalFOV, dAspectRatio, dMinDistance, dMaxDistance);
    mMatrix.scale(QVector3D(1.0, 1.0, -1.0));

    return mMatrix;
}

//-------------------------------------------------------------------------------------------------

QMatrix4x4 CCamera::getQtCameraMatrix(CVector3 vPosition, CVector3 vRotation)
{
    // Création d'une matrice de transformation Qt avec vPosition et vRotation

    QMatrix4x4 mMatrix;

    mMatrix.setToIdentity();
    mMatrix.rotate(Math::Angles::toDeg(-vRotation.Z), QVector3D(0, 0, 1));
    mMatrix.rotate(Math::Angles::toDeg(-vRotation.X), QVector3D(1, 0, 0));
    mMatrix.rotate(Math::Angles::toDeg(-vRotation.Y), QVector3D(0, 1, 0));
    mMatrix.translate(-vPosition.X, -vPosition.Y, -vPosition.Z);

    return mMatrix;
}

//-------------------------------------------------------------------------------------------------

CMatrix4 CCamera::getInternalProjectionMatrix(double dVerticalFOV, double dAspectRatio, double dMinDistance, double dMaxDistance)
{
    return CMatrix4::fromQtMatrix(getQtProjectionMatrix(dVerticalFOV, dAspectRatio, dMinDistance, dMaxDistance));
}

//-------------------------------------------------------------------------------------------------

CMatrix4 CCamera::getInternalCameraMatrix(CVector3 vPosition, CVector3 vRotation)
{
    // Création d'une matrice de transformation interne avec vPosition et vRotation
    // Utilisée pour les tests d'intersection BoundingBox/Frustum entre autres

    CMatrix4 mMatrix;

    mMatrix = CMatrix4().makeRotation(CVector3(0.0, 0.0, -vRotation.Z)) * mMatrix;
    mMatrix = CMatrix4().makeRotation(CVector3(-vRotation.X, 0.0, 0.0)) * mMatrix;
    mMatrix = CMatrix4().makeRotation(CVector3(0.0, -vRotation.Y, 0.0)) * mMatrix;
    mMatrix = CMatrix4().makeTranslation(vPosition * -1.0) * mMatrix;

    return mMatrix;
}

//-------------------------------------------------------------------------------------------------

static bool pointInZone(const CGeoZone& vZone, CVector2 vPoint)
{
    int		i, j = vZone.getLocalPoints().count() - 1;
    bool	oddNodes = false;

    for (i = 0; i < vZone.getLocalPoints().count(); i++)
    {
        if
                (
                 (
                     (vZone.getLocalPoints()[i].Y < vPoint.Y && vZone.getLocalPoints()[j].Y >= vPoint.Y)
                     ||
                     (vZone.getLocalPoints()[j].Y < vPoint.Y && vZone.getLocalPoints()[i].Y >= vPoint.Y)
                     )
                 &&
                 (
                     vZone.getLocalPoints()[i].X <= vPoint.X || vZone.getLocalPoints()[j].X <= vPoint.X
                     )
                 )
        {
            oddNodes ^= (
                        vZone.getLocalPoints()[i].X + (vPoint.Y - vZone.getLocalPoints()[i].Y)
                        / (vZone.getLocalPoints()[j].Y - vZone.getLocalPoints()[i].Y)
                        * (vZone.getLocalPoints()[j].X - vZone.getLocalPoints()[i].X) < vPoint.X
                        );
        }

        j = i;
    }

    return oddNodes;
}

//-------------------------------------------------------------------------------------------------

CGeoZone::EGeoZoneFlag CCamera::categorizePointFromZones(const QVector<CGeoZone>& vZones, CVector2 vPoint)
{
    foreach (const CGeoZone& vZone, vZones)
    {
        if (pointInZone(vZone, vPoint))
        {
            return vZone.getFlag();
        }
    }

    return CGeoZone::gzfUnknown;
}

//-------------------------------------------------------------------------------------------------

CRay3 CCamera::screenPointToWorldRay(CViewport* pViewport, CVector2 vPoint)
{
    // https://math.stackexchange.com/questions/985669/calculating-distance-of-camera-in-3d-environment

    CRay3 rResult;
    CAxis axis(worldRotation());

    double halfW = pViewport->size().X * 0.5;
    double halfH = pViewport->size().Y * 0.5;

    double z0 = halfH * (1.0 + (1.0 / tan(Angles::toRad(m_dFOV * 0.5))));
    double x0 = (vPoint.X - pViewport->position().X) - halfW;
    double y0 = halfH - (vPoint.Y - pViewport->position().Y);

    rResult.vNormal = (axis.Front * z0) + (axis.Right * x0) + (axis.Up * y0);
    rResult.vNormal = rResult.vNormal.normalized();
    rResult.vOrigin = worldPosition();

    return rResult;
}

//-------------------------------------------------------------------------------------------------

void CCamera::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CCamera]"));
    dumpIdent(stream, iIdent, QString("FOV : %1").arg(m_dFOV));
    dumpIdent(stream, iIdent, QString("Focus : %1").arg(m_dFocus));
    dumpIdent(stream, iIdent, QString("Gain : %1").arg(m_dGain));
    dumpIdent(stream, iIdent, QString("Min distance : %1").arg(m_dMinDistance));
    dumpIdent(stream, iIdent, QString("Max distance : %1").arg(m_dMaxDistance));

    CPhysicalComponent::dump(stream, iIdent);
}
