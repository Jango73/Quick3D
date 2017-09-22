
// Qt
#include <QFileDialog>

// Quick3D
#include "CConsoleBoard.h"
#include "CQ3DConstants.h"
#include "CComponentFactory.h"
#include "CComponentLoader.h"
#include "CTerrain.h"
#include "CWorldChunk.h"
#include "CVehicle.h"
#include "CWorldTerrain.h"
#include "CController.h"

// Application
#include "Quick3DTest.h"

#ifdef _MSC_VER
#include "GeneratedFiles/ui_ExportDialog.h"
#else
#include "ui_ExportDialog.h"
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
Quick3DTest::Quick3DTest(QString sSceneFileName, QWidget *parent)
    : QMainWindow(parent)
    #else
Quick3DTest::Quick3DTest(QString sSceneFileName, QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
    #endif
    , m_tTimer(this)
    , m_bProcessEvents(true)
    , m_bRun(true)
{
    // CConsoleBoard::getInstance()->start();

    ui.setupUi(this);

    CComponentFactory::getInstance()->registerCoreComponents();

    m_pScene = new CGLWidgetScene();
    m_pView = new CView(m_pScene, ui.Render1);

    // Gestion des évènements
    connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

    connect(ui.actionLoad_scene, SIGNAL(triggered()), this, SLOT(onLoadSceneClicked()));
    connect(ui.actionClear_scene, SIGNAL(triggered()), this, SLOT(onClearSceneClicked()));
    connect(ui.actionExport_terrain, SIGNAL(triggered()), this, SLOT(onExportTerrainClicked()));
    connect(ui.actionGenerate_matrix, SIGNAL(triggered()), this, SLOT(onGenerateMatrixClicked()));
    connect(ui.actionDump_scene, SIGNAL(triggered()), this, SLOT(onDumpSceneClicked()));

    connect(ui.m_sTime, SIGNAL(valueChanged(int)), this, SLOT(onTimeChanged(int)));
    connect(ui.m_sFogLevel, SIGNAL(valueChanged(int)), this, SLOT(onFogLevelChanged(int)));
    connect(ui.m_sWindLevel, SIGNAL(valueChanged(int)), this, SLOT(onWindLevelChanged(int)));
    connect(ui.m_sShaderQuality, SIGNAL(valueChanged(int)), this, SLOT(onShaderQualityChanged(int)));
    connect(ui.m_sTerrainRes, SIGNAL(valueChanged(int)), this, SLOT(onTerrainResChanged(int)));
    connect(ui.m_sMoveSpeed, SIGNAL(valueChanged(int)), this, SLOT(onMoveSpeedChanged(int)));
    connect(ui.m_sOverlookFOV, SIGNAL(valueChanged(int)), this, SLOT(onOverlookFOVChanged(int)));

    connect(ui.m_chkRain, SIGNAL(clicked()), this, SLOT(onRainClicked()));
    connect(ui.m_chkWideFOV, SIGNAL(clicked()), this, SLOT(onWideFOVClicked()));
    connect(ui.m_chkNarrowFOV, SIGNAL(clicked()), this, SLOT(onNarrowFOVClicked()));
    connect(ui.m_chkIR, SIGNAL(clicked()), this, SLOT(onIRClicked()));
    connect(ui.m_chkStreamView1, SIGNAL(clicked()), this, SLOT(onStreamView1Clicked()));
    connect(ui.m_cbViews1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onViews1IndexChanged(const QString&)));
    connect(ui.m_cbControllable, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onControllableIndexChanged(const QString&)));
    connect(ui.m_chkDebug, SIGNAL(clicked()), this, SLOT(onDebugClicked()));
    connect(ui.m_chkBoundsOnly, SIGNAL(clicked()), this, SLOT(onBoundsOnlyClicked()));
    connect(ui.m_chkNormalsOnly, SIGNAL(clicked()), this, SLOT(onNormalsOnlyClicked()));
    connect(ui.m_chkDepth, SIGNAL(clicked()), this, SLOT(onDepthClicked()));
    connect(ui.m_chkOverlook, SIGNAL(clicked()), this, SLOT(onOverlookClicked()));
    connect(ui.m_chkEdit, SIGNAL(clicked()), this, SLOT(onEditClicked()));

    connect(ui.m_bReset, SIGNAL(clicked()), this, SLOT(onResetClicked()));

    loadScene(QCoreApplication::applicationDirPath() + "/" + sSceneFileName);

    m_sExportPath = QCoreApplication::applicationDirPath() + "/Export";

    if (!QDir().exists(m_sExportPath))
    {
        QDir().mkpath(m_sExportPath);
    }

    m_tPreviousTime = QDateTime::currentDateTime();

    m_tTimer.start(5);
}

//-------------------------------------------------------------------------------------------------

Quick3DTest::~Quick3DTest()
{
    delete m_pScene;

    CComponentFactory::killInstance();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::loadScene(QString sFileName)
{
    LOG_METHOD_DEBUG("Instanciating scene...");

    m_pScene->clear();

    m_pScene->viewports()[0] = new CViewport(m_pScene, false);
    m_pScene->viewports()[0]->setEnabled(true);

    //-----------------------------------------------
    // Load components

    LOG_METHOD_DEBUG("Loading components...");

    QVector<QSP<CComponent> > vComponents = CComponentLoader::getInstance()->load(sFileName, m_pScene);

    m_pScene->init(vComponents);

    //-----------------------------------------------

    onResize();

    fillObjectsCombo();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QTimer::singleShot(0, this, SLOT(onResize()));
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onResize()
{
    if (m_pScene != nullptr && m_pScene->viewports().count() > 0)
    {
        m_pView->setGeometry(0, 0, ui.Render1->width(), ui.Render1->height());
        m_pScene->setGeometry(0, 0, m_pView->width(), m_pView->height());
        m_pScene->viewports()[0]->setPosition(CVector2(0.0, 0.0));
        m_pScene->viewports()[0]->setSize(CVector2((double) ui.Render1->width(), (double) ui.Render1->height()));
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillObjectsCombo()
{
    m_bProcessEvents = false;

    ui.m_cbViews1->clear();
    ui.m_cbControllable->clear();

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        fillCameraCombo(pComponent.data());
    }

    ui.m_cbViews1->setCurrentIndex(-1);

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        fillControlableCombo(pComponent.data());
    }

    ui.m_cbControllable->setCurrentIndex(-1);

    m_bProcessEvents = true;
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillCameraCombo(CComponent* pComponent)
{
    if (pComponent->isCamera())
    {
        QString sQualifiedName = pComponent->qualifiedName();

        ui.m_cbViews1->addItem(sQualifiedName);
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        fillCameraCombo(pChild.data());
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillControlableCombo(CComponent* pComponent)
{
    if (pComponent->isRootObject() || pComponent->isCamera())
    {
        QString sQualifiedName = pComponent->qualifiedName();

        ui.m_cbControllable->addItem(sQualifiedName);
    }

    foreach (QSP<CComponent> pChild, pComponent->childComponents())
    {
        fillControlableCombo(pChild.data());
    }
}

//-------------------------------------------------------------------------------------------------

int Quick3DTest::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

//-------------------------------------------------------------------------------------------------

QString Quick3DTest::getDateString() const
{
    QDateTime tTime = QDateTime::currentDateTime();

    return QString("%1-%2-%3_%4-%5-%6")
            .arg(tTime.date().year())
            .arg(tTime.date().month())
            .arg(tTime.date().day())
            .arg(tTime.time().hour())
            .arg(tTime.time().minute())
            .arg(tTime.time().second());
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTimer()
{
    m_tTimer.stop();

    if (m_pScene != nullptr && m_bRun == true)
    {
        QDateTime tCurrentTime = QDateTime::currentDateTime();
        double dDeltaTime = (double) m_tPreviousTime.msecsTo(tCurrentTime) / 1000.0;
        m_tPreviousTime = tCurrentTime;

        m_pScene->updateScene(dDeltaTime);
        m_pView->update(dDeltaTime);

        ui.m_lInfo->setText(m_pScene->debugInfo());
    }

    m_tTimer.start();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onLoadSceneClicked()
{
    m_bRun = false;

    QString sFileName = QFileDialog::getOpenFileName(this, "Open XML Scene", ".", tr("XML Files (*.xml)"));

    if (sFileName.isEmpty() == false)
    {
        loadScene(sFileName);
    }

    m_bRun = true;
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onClearSceneClicked()
{
    m_pScene->clear();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onExportTerrainClicked()
{
    Ui::ExportDialog ExportDialog;
    QDialog dialog;

    ExportDialog.setupUi(&dialog);

    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        CWorldTerrain* pTerrain = nullptr;

        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            if (pComponent->getClassName() == ClassName_CWorldTerrain)
            {
                pTerrain = dynamic_cast<CWorldTerrain*>(pComponent.data());

                break;
            }
        }

        if (pTerrain != nullptr)
        {
            double dStartLat = ExportDialog.m_tbStartLat->displayText().toDouble();
            double dStartLon = ExportDialog.m_tbStartLon->displayText().toDouble();
            double dSizeZ = ExportDialog.m_tbSizeZ->displayText().toDouble();
            double dSizeX = ExportDialog.m_tbSizeX->displayText().toDouble();
            double dTileSizeM = ExportDialog.m_tbTileSizeM->displayText().toDouble();
            int iPointsPerTile = ExportDialog.m_tbPointsPerTile->displayText().toInt();
            double dHeightOffset = ExportDialog.m_tbHeightOffset->displayText().toDouble();

            double dPointSpacing = dTileSizeM / (double) iPointsPerTile;

            CGeoloc gStart(dStartLat, dStartLon, 0.0);

            double dCurrentX = 0.0;
            double dCurrentZ = 0.0;

            while (dCurrentZ < dSizeZ)
            {
                while (dCurrentX < dSizeX)
                {
                    QString sFilename = m_sExportPath + "/" +
                            QString("Terrain_Z%1_X%2.raw")
                            .arg((long) dCurrentZ, (int) 6, (int) 10, QChar('0'))
                            .arg((long) dCurrentX, (int) 6, (int) 10, QChar('0'));

                    QFile outputFile(sFilename);

                    double dMaxAltitude = 0.0;

                    if (outputFile.open(QIODevice::WriteOnly))
                    {
                        for (int iZ = 0; iZ < iPointsPerTile; iZ++)
                        {
                            for (int iX = 0; iX < iPointsPerTile; iX++)
                            {
                                double dPointX = dCurrentX + dPointSpacing * (double) iX;
                                double dPointZ = dCurrentZ + dPointSpacing * (double) iZ;

                                CGeoloc gPoint(gStart, Math::CVector3(dPointX, 0.0, dPointZ));

                                double dAltitude = dHeightOffset + pTerrain->heights()->getHeightAt(gPoint);
                                if (dAltitude < 0.0) dAltitude = 0.0;

                                if (dAltitude > dMaxAltitude) dMaxAltitude = dAltitude;

                                short sAltitude = (short) dAltitude;

                                outputFile.write((const char*) &sAltitude, sizeof(short));
                            }
                        }

                        outputFile.close();
                    }

                    if (dMaxAltitude < 0.1)
                    {
                        QFile::remove(sFilename);
                    }

                    dCurrentX += dTileSizeM;
                }

                dCurrentZ += dTileSizeM;
                dCurrentX = 0.0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onGenerateMatrixClicked()
{
    CCamera* pCamera = nullptr;

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->isCamera())
        {
            pCamera = dynamic_cast<CCamera*>(pComponent.data());
            break;
        }
    }

    if (pCamera != nullptr)
    {
        // Dump de la scène

        QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");
        if (dump.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&dump);
            m_pScene->dump(stream, 0);
            dump.close();
        }

        // Compute panoramic matrix

        QVector<CGeoZone> vZones;
        QVector<double> vDepth;
        QVector<char> vDetection;
        QVector<char> vEdges;
        QImage imgDepthImage;
        QImage imgDetectionImage;
        QImage imgContourImage;

        CPanoramicMatrixParams tParams(vZones, vDepth, vDetection, vEdges, imgDepthImage, imgDetectionImage, imgContourImage);

        tParams.m_sResolution				= QSize(600, 200);				// Resolution
        tParams.m_gCameraPosition			= pCamera->geoloc();			// Camera position
        tParams.m_dCameraTrueHeadingDegrees	= 0.0;							// True heading
        tParams.m_vStartPanTiltDegrees		= CVector2(-60.0, -180.0);		// Start tilt and pan
        tParams.m_vEndPanTiltDegrees		= CVector2(60.0, 180.0);			// End tilt and pan
        tParams.m_vPanTiltOffsetDegrees		= CVector2(0.0, 0.0); 			// Tilt and pan offset

        pCamera->renderDepth_RayTraced(m_pScene, 10000.0, tParams, nullptr);

        if (vDepth.count() > 0)
        {
            QImage img(tParams.m_sResolution, QImage::Format_RGB888);

            for (int y = 0; y < tParams.m_sResolution.height(); y++)
            {
                for (int x = 0; x < tParams.m_sResolution.width(); x++)
                {
                    double dValue = vDepth[y * tParams.m_sResolution.width() + x];
                    uint uiRGB = 0;

                    double dNormValue = dValue / 20.0;
                    if (dNormValue < 0.0) dNormValue = 255.0;
                    if (dNormValue > 255.0) dNormValue = 255.0;
                    int iIntValue = (int) dNormValue;

                    uiRGB = iIntValue;

                    img.setPixel(x, y, uiRGB);
                }
            }

            img.save("Test.png", "PNG");
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDumpSceneClicked()
{
    // Dump de la scène

    QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");

    if (dump.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&dump);
        m_pScene->dump(stream, 0);
        dump.close();
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTimeChanged(int iValue)
{
    m_pScene->setTimeOfDay(QTime(iValue, 0, 0));
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onFogLevelChanged(int iValue)
{
    m_pScene->setFogLevel((double) iValue / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onWindLevelChanged(int iValue)
{
    m_pScene->setWindLevel((double) iValue / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onShaderQualityChanged(int iValue)
{
    m_pScene->setShaderQuality((double) iValue / 100.0);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onTerrainResChanged(int iValue)
{
    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->getClassName() == ClassName_CWorldTerrain)
        {
            CWorldTerrain* pTerrain = dynamic_cast<CWorldTerrain*>(pComponent.data());

            if (pTerrain != nullptr)
            {
                pTerrain->setTerrainResolution(iValue);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onMoveSpeedChanged(int iValue)
{
    if (m_pScene->controller() != nullptr)
    {
        m_pScene->controller()->setMoveSpeed((double) iValue / 100.0);
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onOverlookFOVChanged(int iValue)
{
    m_pScene->setOverlookFOV((double) iValue);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onRainClicked()
{
    m_pScene->setRainVisible(ui.m_chkRain->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onWideFOVClicked()
{
    m_pScene->forceWideFOV(ui.m_chkWideFOV->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onNarrowFOVClicked()
{
    m_pScene->forceSmallFOV(ui.m_chkNarrowFOV->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onIRClicked()
{
    m_pScene->forceIR(ui.m_chkIR->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onStreamView1Clicked()
{
    if (m_pScene->viewports().count() > 0)
    {
        m_pScene->viewports()[0]->setStreamView(ui.m_chkStreamView1->checkState() == Qt::Checked);
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onViews1IndexChanged(const QString& sName)
{
    if (m_bProcessEvents && m_pScene->viewports().count() > 0)
    {
        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(sName);

            if (pFound && pFound->isCamera())
            {
                m_pScene->viewports()[0]->setCamera(QSP_CAST(CCamera, pFound));
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onControllableIndexChanged(const QString& sName)
{
    if (m_bProcessEvents)
    {
        foreach (QSP<CComponent> pComponent, m_pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(sName);

            if (pFound && pFound->controller() != nullptr)
            {
                m_pScene->setController(pFound->controller());
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onResetClicked()
{
    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (pComponent->getClassName() == ClassName_CTerrestrialVehicle || pComponent->getClassName() == ClassName_CSeaVehicle)
        {
            QSP<CVehicle> pVehicle = QSP_CAST(CVehicle, pComponent);

            if (pVehicle != nullptr)
            {
                pVehicle->resetTrajectory();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDebugClicked()
{
    m_pScene->setDebugMode(ui.m_chkDebug->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onBoundsOnlyClicked()
{
    m_pScene->setBoundsOnly(ui.m_chkBoundsOnly->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onNormalsOnlyClicked()
{
    m_pScene->setNormalsOnly(ui.m_chkNormalsOnly->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onDepthClicked()
{
    m_pScene->setDepthComputing(ui.m_chkDepth->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onOverlookClicked()
{
    m_pScene->setOverlookScene(ui.m_chkOverlook->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onEditClicked()
{
    m_pScene->setEditMode(ui.m_chkEdit->checkState() == Qt::Checked);
}
