
// Qt
#include <QFileDialog>

// Quick3D
#include "CConsoleBoard.h"
#include "CQ3DConstants.h"
#include "CComponent.h"
#include "CComponentFactory.h"
#include "CComponentLoader.h"
#include "CTerrain.h"
#include "CWorldChunk.h"
#include "CVehicle.h"
#include "CAutoTerrain.h"
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
, m_FPS(100)
, m_bProcessEvents(true)
, m_bRun(true)
{
	LOG_DEBUG("Quick3DTest::Quick3DTest()");

	// CConsoleBoard::getInstance()->start();

	ui.setupUi(this);

	CComponentFactory::getInstance()->registerCoreComponents();

	m_pView = new CView(ui.Render1);
	m_pScene = new CGLWidgetScene();
	m_pView->setScene(m_pScene);

	// Gestion des évènements
	connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

	connect(ui.actionLoad_scene, SIGNAL(triggered()), this, SLOT(onLoadSceneClicked()));
	connect(ui.actionExport_terrain, SIGNAL(triggered()), this, SLOT(onExportTerrainClicked()));
	connect(ui.actionGenerate_matrix, SIGNAL(triggered()), this, SLOT(onGenerateMatrixClicked()));
	connect(ui.actionDump_scene, SIGNAL(triggered()), this, SLOT(onDumpSceneClicked()));

	connect(ui.m_sTime, SIGNAL(valueChanged(int)), this, SLOT(onTimeChanged(int)));
	connect(ui.m_sFogLevel, SIGNAL(valueChanged(int)), this, SLOT(onFogLevelChanged(int)));
	connect(ui.m_sWindLevel, SIGNAL(valueChanged(int)), this, SLOT(onWindLevelChanged(int)));
	connect(ui.m_sShaderQuality, SIGNAL(valueChanged(int)), this, SLOT(onShaderQualityChanged(int)));
	connect(ui.m_sTerrainRes, SIGNAL(valueChanged(int)), this, SLOT(onTerrainResChanged(int)));
	connect(ui.m_sMoveSpeed, SIGNAL(valueChanged(int)), this, SLOT(onMoveSpeedChanged(int)));

	connect(ui.m_chkRain, SIGNAL(clicked()), this, SLOT(onRainClicked()));
	connect(ui.m_chkWideFOV, SIGNAL(clicked()), this, SLOT(onWideFOVClicked()));
	connect(ui.m_chkNarrowFOV, SIGNAL(clicked()), this, SLOT(onNarrowFOVClicked()));
	connect(ui.m_chkIR, SIGNAL(clicked()), this, SLOT(onIRClicked()));
	connect(ui.m_chkStreamView1, SIGNAL(clicked()), this, SLOT(onStreamView1Clicked()));
	connect(ui.m_cbViews1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onViews1IndexChanged(const QString&)));
	connect(ui.m_cbControllable, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onControllableIndexChanged(const QString&)));
	connect(ui.m_chkDebug, SIGNAL(clicked()), this, SLOT(onDebugClicked()));
	connect(ui.m_chkBoundsOnly, SIGNAL(clicked()), this, SLOT(onBoundsOnlyClicked()));
	connect(ui.m_chkDepth, SIGNAL(clicked()), this, SLOT(onDepthClicked()));

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
	LOG_DEBUG("Quick3DTest::~Quick3DTest()");

	CComponentFactory::killInstance();
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::loadScene(QString sFileName)
{
	LOG_DEBUG("Quick3DTest::Quick3DTest() : instanciating scene...");

	m_pScene->clear();

	m_pScene->getViewports()[0] = new CViewport(m_pScene, true);
	m_pScene->getViewports()[0]->setEnabled(true);

	//-----------------------------------------------
	// Chargement des composants

	LOG_DEBUG("Quick3DTest::Quick3DTest() : loading components...");

	QVector<CComponent*> vComponents = CComponentLoader::getInstance()->load(m_pScene, sFileName);

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
	if (m_pScene != NULL)
	{
		m_pView->setGeometry(0, 0, ui.Render1->width(), ui.Render1->height());
		m_pScene->setGeometry(0, 0, m_pView->width(), m_pView->height());
		m_pScene->getViewports()[0]->setPosition(CVector2(0.0, 0.0));
		m_pScene->getViewports()[0]->setSize(CVector2((double) ui.Render1->width(), (double) ui.Render1->height()));
	}
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillObjectsCombo()
{
	m_bProcessEvents = false;

	ui.m_cbViews1->clear();
	ui.m_cbControllable->clear();

	foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
	{
		fillCameraCombo(pComponent.data());
	}

	ui.m_cbViews1->setCurrentIndex(-1);

	foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
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
		QString sQualifiedName = pComponent->getQualifiedName();

		ui.m_cbViews1->addItem(sQualifiedName);
	}

	foreach (CComponent* pChild, pComponent->getChildren())
	{
		fillCameraCombo(pChild);
	}
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::fillControlableCombo(CComponent* pComponent)
{
	if (pComponent->isRootObject() || pComponent->isCamera())
	{
		QString sQualifiedName = pComponent->getQualifiedName();

		ui.m_cbControllable->addItem(sQualifiedName);
	}

	foreach (CComponent* pChild, pComponent->getChildren())
	{
		fillControlableCombo(pChild);
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

	if (m_pScene != NULL && m_bRun == true)
	{
		QDateTime tCurrentTime = QDateTime::currentDateTime();
		double dDeltaTime = (double) m_tPreviousTime.msecsTo(tCurrentTime) / 1000.0;
		m_tPreviousTime = tCurrentTime;

		m_FPS.append(1.0 / dDeltaTime);

		if (dDeltaTime < 0.0) dDeltaTime = 0.0;
		if (dDeltaTime > 2.0) dDeltaTime = 2.0;

		m_pScene->updateScene(dDeltaTime);
		m_pView->update(dDeltaTime);

		CGeoloc ViewGeoloc;
		CVector3 ViewRotation;
		CVector3 ControledVelocity;
		CVector3 ControledTorque;
		CVector3 Acceleration;
		CVector3 TorqueAcceleration;
		double dSpeedMS = 0.0;

		if (m_pScene->getController() != NULL && m_pScene->getController()->getPositionTarget() != NULL)
		{
			CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(m_pScene->getController()->getPositionTarget()->getRoot());

			if (pPhysical != NULL)
			{
				ViewGeoloc = pPhysical->getGeoloc();
				ControledVelocity = pPhysical->getVelocity_ms();
				ControledTorque = pPhysical->getAngularVelocity_rs();
				dSpeedMS = ControledVelocity.getMagnitude();
			}
		}

		if (m_pScene->getController() != NULL && m_pScene->getController()->getRotationTarget() != NULL)
		{
			CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(m_pScene->getController()->getPositionTarget()->getRoot());

			if (pPhysical != NULL)
			{
				ViewRotation = pPhysical->getOriginRotation();
			}
		}

		QString sInfo = QString(
			"FPS %1 - LLA (%2, %3, %4) Rotation (%5, %6, %7) Kts %8 \n"
			"Physics Vel (%9, %10, %11) Torque (%12, %13, %14) \n"
			"Drawn : meshes %15 polys %16 chunks %17 (Existing: components %18, chunks %19, terrains %20) \n"
			)
			.arg((int) m_FPS.getAverage())
			.arg(QString::number(ViewGeoloc.Latitude, 'f', 6))
			.arg(QString::number(ViewGeoloc.Longitude, 'f', 6))
			.arg(QString::number(ViewGeoloc.Altitude, 'f', 1))

			.arg(QString::number(Math::Angles::toDeg(ViewRotation.X), 'f', 2))
			.arg(QString::number(Math::Angles::toDeg(ViewRotation.Y), 'f', 2))
			.arg(QString::number(Math::Angles::toDeg(ViewRotation.Z), 'f', 2))

			.arg(QString::number(dSpeedMS * 1.9438444924406046, 'f', 1))

			.arg(QString::number(ControledVelocity.X, 'f', 2))
			.arg(QString::number(ControledVelocity.Y, 'f', 2))
			.arg(QString::number(ControledVelocity.Z, 'f', 2))
			.arg(QString::number(Math::Angles::toDeg(ControledTorque.X), 'f', 2))
			.arg(QString::number(Math::Angles::toDeg(ControledTorque.Y), 'f', 2))
			.arg(QString::number(Math::Angles::toDeg(ControledTorque.Z), 'f', 2))

			.arg(m_pScene->m_iNumMeshesDrawn)
			.arg(m_pScene->m_iNumPolysDrawn)
			.arg(m_pScene->m_iNumChunksDrawn)
			.arg(CComponent::getNumComponents())
			.arg(CWorldChunk::getNumWorldChunks())
			.arg(CTerrain::getNumTerrains())
			;

		ui.m_lInfo->setText(sInfo);
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

void Quick3DTest::onExportTerrainClicked()
{
	Ui::ExportDialog ExportDialog;
	QDialog dialog;

	ExportDialog.setupUi(&dialog);

	int result = dialog.exec();

	if (result == QDialog::Accepted)
	{
		CAutoTerrain* pTerrain = NULL;

		foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
		{
			if (pComponent->getClassName() == ClassName_CAutoTerrain)
			{
				pTerrain = dynamic_cast<CAutoTerrain*>(pComponent.data());

				break;
			}
		}

		if (pTerrain != NULL)
		{
			double dStartLat = ExportDialog.m_tbStartLat->displayText().toDouble();
			double dStartLon = ExportDialog.m_tbStartLon->displayText().toDouble();
			double dSizeZ = ExportDialog.m_tbSizeZ->displayText().toDouble();
			double dSizeX = ExportDialog.m_tbSizeX->displayText().toDouble();
			double dTileSizeM = ExportDialog.m_tbTileSizeM->displayText().toDouble();
			int iPointsPerTile = ExportDialog.m_tbPointsPerTile->displayText().toInt();
			double dHeightOffset = ExportDialog.m_tbHeightOffset->displayText().toDouble();

			double dPointSpacing = dTileSizeM / (double) iPointsPerTile;

			Math::CGeoloc gStart(dStartLat, dStartLon, 0.0);

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

								Math::CGeoloc gPoint(gStart, Math::CVector3(dPointX, 0.0, dPointZ));

								double dAltitude = dHeightOffset + pTerrain->getHeights()->getHeightAt(gPoint);
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
	CCamera* pCamera = NULL;

	foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
	{
		if (pComponent->isCamera())
		{
			pCamera = dynamic_cast<CCamera*>(pComponent.data());
			break;
		}
	}

	if (pCamera != NULL)
	{
		// Dump de la scène

		QFile dump(QCoreApplication::applicationDirPath() + "/Scene.dump.txt");
		if (dump.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&dump);
			m_pScene->dump(stream, 0);
			dump.close();
		}

		// Calcul de la matrice panoramique

		QVector<CGeoZone> vZones;
		QVector<double> vDepth;
		QVector<char> vDetection;
		QVector<char> vEdges;
		QImage imgDepthImage;
		QImage imgDetectionImage;
		QImage imgContourImage;

		CPanoramicMatrixParams tParams(vZones, vDepth, vDetection, vEdges, imgDepthImage, imgDetectionImage, imgContourImage);

		tParams.m_sResolution				= QSize(600, 200);				// Resolution
		tParams.m_gCameraPosition			= pCamera->getGeoloc();			// Camera position
		tParams.m_dCameraTrueHeadingDegrees	= 0.0;							// True heading
		tParams.m_vStartPanTiltDegrees		= CVector2(-60.0, -180.0);		// Start tilt and pan
		tParams.m_vEndPanTiltDegrees		= CVector2(60.0, 180.0);			// End tilt and pan
		tParams.m_vPanTiltOffsetDegrees		= CVector2(0.0, 0.0); 			// Tilt and pan offset

		pCamera->renderDepth_RayTraced(m_pScene, 10000.0, tParams, NULL);

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
	foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
	{
		if (pComponent->getClassName() == ClassName_CAutoTerrain)
		{
			CAutoTerrain* pTerrain = dynamic_cast<CAutoTerrain*>(pComponent.data());

			if (pTerrain != NULL)
			{
				pTerrain->setTerrainResolution(iValue);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onMoveSpeedChanged(int iValue)
{
    if (m_pScene->getController() != NULL)
    {
        m_pScene->getController()->setMoveSpeed((double) iValue / 100.0);
    }
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
	m_pScene->getViewports()[0]->setStreamView(ui.m_chkStreamView1->checkState() == Qt::Checked);
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onViews1IndexChanged(const QString& sName)
{
	if (m_bProcessEvents)
	{
		foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
		{
			CComponent* pFound = pComponent->findComponent(sName);

			if (pFound != NULL && pFound->isCamera())
			{
				m_pScene->getViewports()[0]->setCamera(dynamic_cast<CCamera*>(pFound));
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
		foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
		{
			CComponent* pFound = pComponent->findComponent(sName);

			if (pFound != NULL)
			{
				if (pFound->getController() != NULL)
				{
					m_pScene->setController(pFound->getController());
				}

				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void Quick3DTest::onResetClicked()
{
	foreach (QSharedPointer<CComponent> pComponent, m_pScene->getComponents())
	{
		if (pComponent->getClassName() == ClassName_CTerrestrialVehicle || pComponent->getClassName() == ClassName_CSeaVehicle)
		{
			CVehicle* pVehicle = dynamic_cast<CVehicle*>(pComponent.data());

			if (pVehicle != NULL)
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

void Quick3DTest::onDepthClicked()
{
	m_pScene->setDepthComputing(ui.m_chkDepth->checkState() == Qt::Checked);
}
