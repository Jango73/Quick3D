
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
#include "CRenderLayer.h"
#include "CImageUtilities.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------

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

    // Create polygons in local 3D space and viewed from top (X and Z)
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
                // Is the component subject to ray-tracing?
                if (pComponent->isVisible() && pComponent->isRaytracable())
                {
                    RayTracingResult dNewResult = pComponent->intersect(rCameraRay);

                    // Keep the returned distance if less than the one already computed
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

// Do not lower the following numbers, resulting matrix will be deformed otherwise
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

        // Build zone polygons in the local 3D frame viewed from above (X and Z)
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

        // Computation of the size of a mini-render
        // To review since this computation assumes the requested matrix spans 360 by 180 degrees

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

        int iRenderMapSize = 265;	// Power of two

        LOG_METHOD_DEBUG(
                    QString("Rendering %1 x %2 image tiles of %3 pixels resolution")
                    .arg(RENDER_SUBDIVISIONS_PAN)
                    .arg(RENDER_SUBDIVISIONS_TILT)
                    .arg(iRenderMapSize)
                    );

        // Computation of the FOVs
        double dFOV_Pan = 360.0 / (double) RENDER_SUBDIVISIONS_PAN;
        double dFOV_Tilt = 180.0 / (double) RENDER_SUBDIVISIONS_TILT;
        double dFOV_Ratio = dFOV_Tilt / dFOV_Pan;

        // Define the viewport size
        QSize sMapSize(iRenderMapSize, (int) ((double) iRenderMapSize * dFOV_Ratio));

        // Vector holding the size of a cube-map image
        CVector2 vMapSize((double) sMapSize.width(), (double) sMapSize.height());

        // Save the camera settings
        QRect rOldGeometry = pGLWidgetScene->geometry();
        CViewport* pOldViewport = pGLWidgetScene->viewports()[0];
        CViewport* pNewViewport = new CViewport(pGLWidgetScene);
        CVector3 vCameraInitialRotation = rotation();
        CVector3 vCameraAttitude = tParams.m_vAttitude.degreesToRadians() * -1.0;
        CVector3 vCameraHeading(0.0, Math::Angles::toRad(tParams.m_dCameraTrueHeadingDegrees), 0.0);
        double dOldFOV = m_dFOV;

        // Create the viewport used to render the scene
        pGLWidgetScene->setGeometry(0, 0, sMapSize.width(), sMapSize.height());
        pGLWidgetScene->viewports()[0] = pNewViewport;
        pGLWidgetScene->viewports()[0]->setSize(vMapSize);
        pGLWidgetScene->viewports()[0]->setCamera(QSP<CCamera>(this));
        pGLWidgetScene->viewports()[0]->setEnabled(true);
        pGLWidgetScene->setDepthComputing(true);
        pGLWidgetScene->setShaderQuality(0.0);
        pGLWidgetScene->updateScene(1.0);

        // Create the reference cube-map
        QImage imgImage(QSize(sMapSize.width() * RENDER_SUBDIVISIONS_PAN, sMapSize.height() * RENDER_SUBDIVISIONS_TILT), QImage::Format_RGB888);

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:prepare");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:render");

        // Set the horizontal camera FOV (vertical FOV follows the viewport size)
        m_dFOV = dFOV_Tilt;

        double dHalfFOV_Pan = dFOV_Pan * 0.5;
        double dHalfFOV_Tilt = dFOV_Tilt * 0.5;

        // Rendering of each image portion
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

                // Get the rotation of the current image
                CVector3 vCurrentRotationRadians = CVector3(Math::Angles::toRad(dCurrentTilt), Math::Angles::toRad(dCurrentPan), 0.0);

                // Compute the final camera angles through an axis
                Math::CAxis anAxis;
                anAxis = anAxis.rotate(vCurrentRotationRadians);
                anAxis = anAxis.rotate(CVector3(0.0, vCameraAttitude.Y, 0.0));
                anAxis = anAxis.rotate(CVector3(vCameraAttitude.X, 0.0, 0.0));
                anAxis = anAxis.rotate(CVector3(0.0, 0.0, vCameraAttitude.Z));
                anAxis = anAxis.rotate(vCameraHeading);

                // Apply the axis angles to the camera
                setRotation(anAxis.eulerAngles());

                // OpenGL rendering
                pGLWidgetScene->paintGL();

                // Copy the rendered image into the full image
                CImageUtilities::blitWhole(
                            imgImage,
                            QPoint(sMapSize.width() * iPanIndex, sMapSize.height() * iTiltIndex),
                            getFrameBuffer(pGLWidgetScene, sMapSize)
                            );
            }
        }

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:render");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:postprocess");

        // For tests: save the final image as rendered by OpenGL
        // imgImage.save("d:\\Pano.png");

        // Collect the distances
        for (int iTilt = 0; iTilt < tParams.m_sResolution.height(); iTilt++)
        {
            for (int iPan = 0; iPan < tParams.m_sResolution.width(); iPan++)
            {
                // Computation of the current pan and tilt in image space

                double dPanNormalized = (double) iPan / (double) tParams.m_sResolution.width();
                double dTiltNormalized = (double) iTilt / (double) tParams.m_sResolution.height();

                double dCurrentPan = dPanNormalized * (tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y);
                double dCurrentTilt = dTiltNormalized * (tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X);

                // Adjustments
                dCurrentPan = dCurrentPan + tParams.m_vStartPanTiltDegrees.Y;
                dCurrentTilt = dCurrentTilt + tParams.m_vStartPanTiltDegrees.X;

                dCurrentPan += 180.0;
                dCurrentTilt += 90.0;

                dCurrentPan = Math::Angles::clipAngleDegree(dCurrentPan);
                dCurrentTilt = Math::Angles::clipAngleDegree(dCurrentTilt);

                dCurrentPan = (dCurrentPan / 360.0) * imgImage.width();
                dCurrentTilt = (dCurrentTilt / 180.0) * imgImage.height();

                CVector2 vCoords(dCurrentPan, dCurrentTilt);

                // Initialize output data
                double dDistance = -1.0;
                CGeoZone::EGeoZoneFlag eDetectionFlag = CGeoZone::gzfUnknown;
                int iDotRayNormal = -127;

                if (
                        vCoords.X >= 0.0 && vCoords.X < (double) imgImage.width() &&
                        vCoords.Y >= 0.0 && vCoords.Y < (double) imgImage.height()
                        )
                {
                    // Get the pixel, contains the distance and viewing angle
                    QRgb rgbPixel = imgImage.pixel((int) vCoords.X, (int) vCoords.Y);

                    // 0xFF = infinity
                    if ((rgbPixel & 0xFF) == 0xFF)
                    {
                        dDistance = -1.0;
                    }
                    else
                    {
                        // Get the distance
                        dDistance = ((double) ((rgbPixel >> 16) & 0xFF) / 255.0) * 10000.0;

                        // Dot product between "view" vector and terrain normal vector
                        // In other words, terrain incidence angle
                        iDotRayNormal = (rgbPixel >> 8) & 0xFF;

                        // Here we work in the topocentric frame

                        // Compute current pan and tilt in camera space
                        dCurrentPan = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.Y + (dPanNormalized * (tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y)));
                        dCurrentTilt = Math::Angles::toRad(tParams.m_vStartPanTiltDegrees.X + (dTiltNormalized * (tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X)));

                        // Create the ray for the current point
                        CRay3 rCameraRay;

                        // The ray points to +Z
                        rCameraRay.vOrigin = CVector3(0.0, 0.0, 0.0);
                        rCameraRay.vNormal = CVector3(0.0, 0.0, 1.0);

                        // Apply the current euler angles to the ray
                        rCameraRay = CMatrix4().makeRotation(CVector3(dCurrentTilt, 0.0, 0.0)) * rCameraRay;
                        rCameraRay = CMatrix4().makeRotation(CVector3(0.0, dCurrentPan, 0.0)) * rCameraRay;

                        // Apply the camera's heading to the ray
                        rCameraRay = CMatrix4().makeRotation(vCameraHeading) * rCameraRay;

                        // Compute the intersection point
                        CVector3 vIntersectionPoint3D = rCameraRay.vOrigin + rCameraRay.vNormal * dDistance;

                        // Get the flag related to detection zones
                        eDetectionFlag = categorizePointFromZones(
                                    tParams.m_vZones,
                                    CVector2(vIntersectionPoint3D.X, vIntersectionPoint3D.Z)
                                    );
                    }
                }

                // Fill the matrices
                tParams.m_vDetection.append((char) eDetectionFlag);
                tParams.m_vEdges.append((char) iDotRayNormal);
                tParams.m_vDepth.append(dDistance);
            }
        }

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:postprocess");

        START_SAMPLE("CCamera::renderDepth_CubeMapped:cleanup");

        // Get the scene settings
        pGLWidgetScene->setGeometry(rOldGeometry);
        pGLWidgetScene->viewports()[0] = pOldViewport;
        pGLWidgetScene->setDepthComputing(false);
        pGLWidgetScene->setShaderQuality(0.5);

        // Restore camera settings
        setRotation(vCameraInitialRotation);
        m_dFOV = dOldFOV;

        // Destroy the temporary viewport
        delete pNewViewport;

        STOP_SAMPLE("CCamera::renderDepth_CubeMapped:cleanup");
    }
}
