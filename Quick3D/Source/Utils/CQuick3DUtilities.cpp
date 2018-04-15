
// Qt
#include <QGraphicsEffect>
#include <QPainter>

// qt-plus
#include "CLogger.h"
#include "CTimeSampler.h"

// Application
#include "CQuick3DUtilities.h"
#include "CImageFilter_Matrix.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CQuick3DUtilities::CQuick3DUtilities()
    : m_pCMScene(nullptr)
{
}

//-------------------------------------------------------------------------------------------------

CQuick3DUtilities::~CQuick3DUtilities()
{
    if (m_pCMScene != nullptr)
    {
        // Destruction des éléments 3D
        delete m_pCMScene;
        delete m_pCMField;
    }
}

//-------------------------------------------------------------------------------------------------

void CQuick3DUtilities::prepareSceneForMatrices_CubeMapped(CPanoramicMatrixParams& tParams)
{
    if (m_pCMScene == nullptr)
    {
        // Création de la scène
        m_pCMScene = new CGLWidgetScene();

        // Création du champ de hauteurs
        if (tParams.m_sPathToBILData.isEmpty() == false)
        {
            m_pCMField = new CBILField(CXMLNode(), tParams.m_sPathToBILData, 0.0);
        }
        else
        {
            m_pCMField = new CSRTMField(CXMLNode(), tParams.m_sPathToSTRMData, 0.0);
        }

        // Création de la caméra
        m_pCMCamera = QSP<CCamera>(new CCamera(m_pCMScene));
        m_pCMCamera->setGeoloc(CGeoloc(tParams.m_gCameraPosition.Latitude, tParams.m_gCameraPosition.Longitude, 0.0));

        // Création du viewport
        m_pCMScene->viewports()[0] = new CViewport(m_pCMScene);
        m_pCMScene->viewports()[0]->setSize(Math::CVector2(512, 512));
        m_pCMScene->viewports()[0]->setCamera(m_pCMCamera);
        m_pCMScene->viewports()[0]->setEnabled(true);

        // Création du générateur de terrain
        m_pCMTerrain = QSP<CWorldTerrain>(new CWorldTerrain(m_pCMScene, tParams.m_gCameraPosition, m_pCMField, true));

        // double dAltitude = m_pCMTerrain->getHeightAt(tParams.m_gCameraPosition);

        // Ajout terrain et caméra à la scène, seront détruits automatiquement
        QVector<QSP<CComponent> > vComponents;
        vComponents.append(m_pCMTerrain);
        vComponents.append(m_pCMCamera);
        m_pCMScene->init(vComponents);

        // Dump de la scène pour tests
        /*
        QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");
        if (dump.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&dump);
            m_pCMScene->dump(stream, 0);
            dump.close();
        }
        */
    }
}

//-------------------------------------------------------------------------------------------------

void CQuick3DUtilities::computeSphericalDetectionMatrices_RayTraced(CPanoramicMatrixParams& tParams, IProgressListener* pProgressListener)
{
    // Création de la scène
    C3DScene* pScene = new C3DScene(false);

    CHeightField* pField;

    if (tParams.m_sPathToBILData.isEmpty() == false)
    {
        pField = new CBILField(CXMLNode(), tParams.m_sPathToBILData, 0.0);
    }
    else
    {
        pField = new CSRTMField(CXMLNode(), tParams.m_sPathToSTRMData, 0.0);
    }

    // Création du générateur de terrain
    QSP<CWorldTerrain> pTerrain(new CWorldTerrain(pScene, tParams.m_gCameraPosition, pField, true));

    double dAltitude = pTerrain->getHeightAt(tParams.m_gCameraPosition);

    // Création de la caméra
    QSP<CCamera> pCamera(new CCamera(pScene));
    pCamera->setGeoloc(CGeoloc(tParams.m_gCameraPosition.Latitude, tParams.m_gCameraPosition.Longitude, dAltitude + tParams.m_gCameraPosition.Altitude));
    pCamera->setRotation(CVector3(0.0, Math::Angles::toRad(tParams.m_dCameraTrueHeadingDegrees), 0.0));

    // Ajout terrain et caméra à la scène, seront détruits automatiquement
    QVector<QSP<CComponent> > vComponents;
    vComponents.append(pTerrain);
    vComponents.append(pCamera);
    pScene->init(vComponents);

    // Dump de la scène
    /*
    QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");
    if (dump.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&dump);
        pScene->dump(stream, 0);
        dump.close();
    }
    */

    // Appel méthode de calcul de profondeur et de zones
    pCamera->renderDepth_RayTraced(pScene, 10000.0, tParams, pProgressListener);

    // Destruction des éléments 3D
    delete pScene;
    delete pField;

    computeMatrixImages(tParams);
}

//-------------------------------------------------------------------------------------------------

void CQuick3DUtilities::computeSphericalDetectionMatrices_CubeMapped(CPanoramicMatrixParams& tParams, IProgressListener* pProgressListener)
{
    qDebug() << "Computing matrices via OpenGL - START";

    LOG_METHOD_DEBUG("Setting up scene...");

    START_SAMPLE("computeSphericalDetectionMatrices_CubeMapped:setup");

    prepareSceneForMatrices_CubeMapped(tParams);

    double dAltitude = m_pCMTerrain->getHeightAt(tParams.m_gCameraPosition);

    m_pCMCamera->setGeoloc(CGeoloc(tParams.m_gCameraPosition.Latitude, tParams.m_gCameraPosition.Longitude, dAltitude + tParams.m_gCameraPosition.Altitude));

    STOP_SAMPLE("computeSphericalDetectionMatrices_CubeMapped:setup");

    LOG_METHOD_DEBUG("Rendering matrices...");

    START_SAMPLE("computeSphericalDetectionMatrices_CubeMapped:render");

    // Appel méthode de calcul de profondeur et de zones
    m_pCMCamera->renderDepth_CubeMapped(m_pCMScene, 10000.0, tParams, pProgressListener);

    STOP_SAMPLE("computeSphericalDetectionMatrices_CubeMapped:render");

    // A appeler pour tests
    // computeMatrixImages(tParams);

    qDebug() << "Computing matrices via OpenGL - END";
}

//-------------------------------------------------------------------------------------------------

void CQuick3DUtilities::computeMatrixImages(CPanoramicMatrixParams& tParams)
{
    START_SAMPLE("computeMatrixImages:depth_image");

    // Création de l'image de profondeur
    if (tParams.m_vDepth.count() > 0)
    {
        tParams.m_imgDepthImage = QImage(
                    QSize(tParams.m_sResolution.width(), tParams.m_sResolution.height()),
                    QImage::Format_RGB888
                    );

        for (int y = 0; y < tParams.m_sResolution.height(); y++)
        {
            for (int x = 0; x < tParams.m_sResolution.width(); x++)
            {
                double dValue = tParams.m_vDepth[y * tParams.m_sResolution.width() + x];
                uint uiRGB = 0;
                quint32 iIntValue = 0;

                double dNormValue = dValue / 10.0;

                if (dNormValue >= 0.0)
                {
                    if (dNormValue < 0.0) dNormValue = 0.0;
                    if (dNormValue > 255.0) dNormValue = 255.0;
                    iIntValue = 255 - ((int) dNormValue);
                    iIntValue = iIntValue;
                }
                else
                {
                    iIntValue = 0xFF << 16;
                }

                uiRGB = iIntValue;

                tParams.m_imgDepthImage.setPixel(x, y, uiRGB);
            }
        }

        QPainter painter(&tParams.m_imgDepthImage);
        painter.setPen(Qt::white);

        double dPanSpan = tParams.m_vEndPanTiltDegrees.Y - tParams.m_vStartPanTiltDegrees.Y;
        double dTiltSpan = tParams.m_vEndPanTiltDegrees.X - tParams.m_vStartPanTiltDegrees.X;

        // Graduations en site - 10°
        for (double dTilt = -90.0; dTilt <= 90.0; dTilt += 10.0)
        {
            int x1 = (tParams.m_sResolution.width() / 2) - 100;
            int x2 = (tParams.m_sResolution.width() / 2) + 100;

            int y = (int) (
                        ((dTilt - tParams.m_vStartPanTiltDegrees.X) / dTiltSpan) *
                        (double) tParams.m_sResolution.height()
                        );

            painter.drawLine(x1, y, x2, y);
            painter.drawText(x2 + 10, y, QString::number(dTilt));
        }

        // Graduations en site - 1°
        for (double dTilt = -90.0; dTilt <= 90.0; dTilt += 1.0)
        {
            int x1 = (tParams.m_sResolution.width() / 2) - 50;
            int x2 = (tParams.m_sResolution.width() / 2) + 50;

            int y = (int) (
                        ((dTilt - tParams.m_vStartPanTiltDegrees.X) / dTiltSpan) *
                        (double) tParams.m_sResolution.height()
                        );

            painter.drawLine(x1, y, x2, y);
        }

        // Graduations en gisement - 10°
        for (double dPan = -180.0; dPan <= 180.0; dPan += 10.0)
        {
            int y1 = (tParams.m_sResolution.height() / 2) - (tParams.m_sResolution.height() / 4);
            int y2 = (tParams.m_sResolution.height() / 2) + (tParams.m_sResolution.height() / 4);

            int x = (int) (
                        ((dPan - tParams.m_vStartPanTiltDegrees.Y) / dPanSpan) *
                        (double) tParams.m_sResolution.width()
                        );

            painter.drawLine(x, y1, x, y2);
        }
    }

    STOP_SAMPLE("computeMatrixImages:depth_image");

    START_SAMPLE("computeMatrixImages:detection_image");

    // Création de l'image de détection
    if (tParams.m_vDetection.count() > 0)
    {
        tParams.m_imgDetectionImage = QImage(
                    QSize(tParams.m_sResolution.width(), tParams.m_sResolution.height()),
                    QImage::Format_RGB888
                    );

        for (int y = 0; y < tParams.m_sResolution.height(); y++)
        {
            for (int x = 0; x < tParams.m_sResolution.width(); x++)
            {
                char dValue = tParams.m_vDetection[y * tParams.m_sResolution.width() + x];
                uint uiRGB = 0;

                switch (dValue)
                {
                    case 0 : uiRGB = 0xFF0000; break;
                    case 1 : uiRGB = 0x000000; break;
                    case 2 : uiRGB = 0x00FF00; break;
                    case 3 : uiRGB = 0x0000FF; break;
                }

                tParams.m_imgDetectionImage.setPixel(x, y, uiRGB);
            }
        }
    }

    STOP_SAMPLE("computeMatrixImages:detection_image");

    START_SAMPLE("computeMatrixImages:contours_image");

    // Création de l'image de contours
    if (tParams.m_vEdges.count() > 0)
    {
        tParams.m_imgContourImage = QImage(
                    QSize(tParams.m_sResolution.width(), tParams.m_sResolution.height()),
                    QImage::Format_RGB888
                    );

        // Récupération des pixels depuis tParams.m_vEdges
        for (int y = 0; y < tParams.m_sResolution.height(); y++)
        {
            for (int x = 0; x < tParams.m_sResolution.width(); x++)
            {
                int iData = tParams.m_vEdges[y * tParams.m_sResolution.width() + x];
                QRgb uiRGB = qRgb((int) iData, (int) iData, 0);
                tParams.m_imgContourImage.setPixel(x, y, uiRGB);
            }
        }

        // Création d'une matrice "edges"
        QMatrix3x3 mMatrix;

        mMatrix(0, 0) =  1.0;
        mMatrix(1, 0) =  1.0;
        mMatrix(2, 0) =  1.0;
        mMatrix(0, 1) =  0.0;
        mMatrix(1, 1) =  0.0;
        mMatrix(2, 1) =  0.0;
        mMatrix(0, 2) = -1.0;
        mMatrix(1, 2) = -1.0;
        mMatrix(2, 2) = -1.0;

        // Application de la matrice à l'image
        CImageFilter_Matrix* pMatrix = new CImageFilter_Matrix(mMatrix);
        pMatrix->process(tParams.m_imgContourImage);
    }

    STOP_SAMPLE("computeMatrixImages:contours_image");
}
