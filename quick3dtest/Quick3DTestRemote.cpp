
// Qt
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QRandomGenerator>
#include <QKeyEvent>

// Quick3D
#include "CEnvironmentDefaults.h"

// qt-plus
#include "CTimeSampler.h"

// Quick3D
#include "CConsoleBoard.h"
#include "CQ3DConstants.h"
#include "CComponentFactory.h"
#include "CComponentLoader.h"
#include "CWorkerManager.h"
#include "CTerrain.h"
#include "CWorldChunk.h"
#include "CVehicle.h"
#include "CWheel.h"
#include "CWorldTerrain.h"
#include "CController.h"
#include "CPhysicsWorld.h"
#include "CRemoteControlServer.h"
#include "CUiRemoteControlHandler.h"

// Application
#include "Quick3DTest.h"

#ifdef _MSC_VER
#include "GeneratedFiles/ui_ExportDialog.h"
#else
#include "ui_ExportDialog.h"
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------

/*!
    Selects the first combo item matching sName (exact, then contains), returns its index or -1
*/
int Quick3DTest::selectComboItem(QComboBox* pCombo, const QString& sName)
{
    if (pCombo == nullptr)
    {
        return -1;
    }

    int iIndex = pCombo->findText(sName);

    if (iIndex < 0)
    {
        for (int iItem = 0; iItem < pCombo->count(); iItem++)
        {
            if (pCombo->itemText(iItem).contains(sName) == true)
            {
                iIndex = iItem;
                break;
            }
        }
    }

    if (iIndex >= 0)
    {
        pCombo->setCurrentIndex(iIndex);
    }

    return iIndex;
}

//-------------------------------------------------------------------------------

/*!
    Selects a vehicle through the controllable combo box, like a user click, returns true on success
*/
bool Quick3DTest::selectVehicleByName(const QString& sName)
{
    if (m_pScene == nullptr)
    {
        return false;
    }

    if (selectComboItem(ui.m_cbControllable, sName) < 0)
    {
        return false;
    }

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        QSP<CComponent> pFound = pComponent->findComponent(sName);

        if (pFound != nullptr && pFound->controller() != nullptr)
        {
            return m_pScene->controller() == pFound->controller();
        }
    }

    return false;
}

//-------------------------------------------------------------------------------

/*!
    Selects a camera through the views combo box, like a user click, returns true on success
*/
bool Quick3DTest::selectViewByName(const QString& sName)
{
    if (m_pScene == nullptr || m_pScene->viewports().count() == 0)
    {
        return false;
    }

    if (selectComboItem(ui.m_cbViews1, sName) < 0)
    {
        return false;
    }

    QSP<CCamera> pCamera = m_pScene->viewports()[0]->camera();

    return pCamera != nullptr && pCamera->qualifiedName().contains(sName) == true;
}

//-------------------------------------------------------------------------------

/*!
    Posts a key event (Qt::Key value, 0 = press, 1 = release, 2 = click) to the 3D view, returns true on success
*/
bool Quick3DTest::postKeyToView(int iKey, int iAction)
{
    if (m_pView == nullptr)
    {
        return false;
    }

    if (iAction == 0 || iAction == 2)
    {
        QApplication::postEvent(m_pView, new QKeyEvent(QEvent::KeyPress, iKey, Qt::NoModifier));
    }

    if (iAction == 1 || iAction == 2)
    {
        QApplication::postEvent(m_pView, new QKeyEvent(QEvent::KeyRelease, iKey, Qt::NoModifier));
    }

    return true;
}

//-------------------------------------------------------------------------------

/*!
    Sets the move speed slider like a user drag, returns true on success
*/
bool Quick3DTest::setMoveSpeedSlider(int iValue)
{
    ui.m_sMoveSpeed->setValue(qBound(ui.m_sMoveSpeed->minimum(), iValue, ui.m_sMoveSpeed->maximum()));

    return true;
}

//-------------------------------------------------------------------------------

/*!
    Returns a JSON document describing the vehicle named sName
*/
QString Quick3DTest::vehicleStatusJson(const QString& sName)
{
    if (m_pScene == nullptr)
    {
        return "{\"ok\":false,\"error\":\"no scene\"}";
    }

    QSP<CVehicle> pVehicle;
    QSP<CComponent> pFoundComponent;

    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        QSP<CComponent> pFound = pComponent->findComponent(sName);

        if (pFound != nullptr)
        {
            pFoundComponent = pFound;
            pVehicle = QSP_CAST(CVehicle, pFound);
            break;
        }
    }

    if (pVehicle == nullptr)
    {
        // Non-vehicle components (Man1 is a CTrajectorable) still report their position
        if (pFoundComponent == nullptr)
        {
            return "{\"ok\":false,\"error\":\"vehicle not found\"}";
        }

        CGeoloc gGeoloc = pFoundComponent->geoloc();

        return QString("{\"ok\":true,\"name\":\"%1\",\"physics\":false,\"latitude\":%2,\"longitude\":%3,\"altitude\":%4}")
            .arg(pFoundComponent->name())
            .arg(gGeoloc.Latitude, 0, 'g', 10).arg(gGeoloc.Longitude, 0, 'g', 10).arg(gGeoloc.Altitude, 0, 'g', 10);
    }

    CGeoloc gGeoloc = pVehicle->geoloc();
    CVector3 vRotation = pVehicle->rotation();

    int iContacts = -1;

    CPhysicsWorld* pWorld = m_pScene->physicsWorld();

    if (pWorld != nullptr && pVehicle->hasPhysicsVehicle() == true)
    {
        iContacts = 0;

        QVector<QSP<CWheel> > vWheels = pVehicle->wheels();

        for (int iWheel = 0; iWheel < vWheels.count(); iWheel++)
        {
            if (pWorld->vehicleWheelHasContact(pVehicle->physicsVehicleIndex(), iWheel) == true)
            {
                iContacts++;
            }
        }
    }

    bool bControlled = m_pScene->controller() != nullptr
        && pVehicle->controller() != nullptr
        && m_pScene->controller() == pVehicle->controller();

    return QString("{\"ok\":true,\"name\":\"%1\",\"physics\":%2,\"latitude\":%3,\"longitude\":%4,\"altitude\":%5,"
        "\"rotationX\":%6,\"rotationY\":%7,\"rotationZ\":%8,\"handBrake\":%9,\"wheels\":%10,\"contacts\":%11,\"controlled\":%12}")
        .arg(pVehicle->name())
        .arg(pVehicle->hasPhysicsVehicle() == true ? "true" : "false")
        .arg(gGeoloc.Latitude, 0, 'g', 10).arg(gGeoloc.Longitude, 0, 'g', 10).arg(gGeoloc.Altitude, 0, 'g', 10)
        .arg(vRotation.X, 0, 'g', 10).arg(vRotation.Y, 0, 'g', 10).arg(vRotation.Z, 0, 'g', 10)
        .arg(pVehicle->isHandBrakeEngaged() == true ? "true" : "false")
        .arg(pVehicle->wheels().count())
        .arg(iContacts)
        .arg(bControlled == true ? "true" : "false");
}

//-------------------------------------------------------------------------------

/*!
    Returns a JSON document with the full engine state for the current frame.
    Each object serializes its own state through ISnapshotable, this method only assembles the parts.
*/
QString Quick3DTest::engineSnapshotJson()
{
    if (m_pScene == nullptr)
    {
        return "{\"ok\":false,\"error\":\"no scene\"}";
    }

    QSP<CCamera> pCamera;

    if (m_pScene->viewports().count() > 0)
    {
        pCamera = m_pScene->viewports()[0]->camera();
    }

    if (pCamera == nullptr)
    {
        return "{\"ok\":false,\"error\":\"no camera\"}";
    }

    QString sControlled = ui.m_cbControllable != nullptr ? ui.m_cbControllable->currentText() : QString();
    QString sVehicle = QString("null");
    QString sTerrain = QString("null");

    // One pass over the top-level components collects both the controlled
    // component and the world terrain, instead of scanning twice.
    foreach (QSP<CComponent> pComponent, m_pScene->components())
    {
        if (sControlled.isEmpty() == false && sVehicle == QString("null"))
        {
            QSP<CComponent> pFound = pComponent->findComponent(sControlled);

            if (pFound != nullptr)
            {
                sVehicle = pFound->snapshotJson();
            }
        }

        if (sTerrain == QString("null"))
        {
            QSP<CWorldTerrain> pTerrain = QSP_CAST(CWorldTerrain, pComponent);

            if (pTerrain != nullptr)
            {
                sTerrain = pTerrain->snapshotJson();
            }
        }

        if ((sControlled.isEmpty() == true || sVehicle != QString("null")) && sTerrain != QString("null"))
        {
            break;
        }
    }

    return QString("{\"ok\":true,\"frame\":%1,\"paused\":%2,\"camera\":%3,\"vehicle\":%4,\"terrain\":%5,\"workers\":%6}")
        .arg(m_iFrames)
        .arg(m_bRun == false ? "true" : "false")
        .arg(pCamera->snapshotJson())
        .arg(sVehicle)
        .arg(sTerrain)
        .arg(CWorkerManager::getInstance()->snapshotJson());
}

//-------------------------------------------------------------------------------

/*!
    Dumps the engine snapshot to a uniquely named file, never overwriting. \br\br
    Files land in the snapshots/ folder of the binary folder as
    snapshot-YYYYMMDD-hhmmss-f<frame>.json, with a counter suffix when the
    name is already taken.
*/
void Quick3DTest::onSnapshotClicked()
{
    // Snapshots resolve against the binary folder like scenes and models do, never against the working folder.
    QString sFolder = QCoreApplication::applicationDirPath() + "/" + SnapshotsFolderName;

    QDir().mkpath(sFolder);

    QString sStamp = QDateTime::currentDateTime().toString(SnapshotStampFormat);
    QString sBase = QString(SnapshotFilePattern).arg(sStamp).arg(m_iFrames);
    QString sUnique = QString("%1/%2.json").arg(sFolder).arg(sBase);

    int iAttempt = 0;

    while (QFile::exists(sUnique) && iAttempt < MaxSnapshotAttempts)
    {
        iAttempt++;
        sUnique = QString("%1/%2-%3.json").arg(sFolder).arg(sBase).arg(iAttempt);
    }

    QFile cFile(sUnique);

    if (cFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cFile.write(engineSnapshotJson().toUtf8());
        cFile.close();

        LOG_METHOD_INFO(QString("Snapshot saved to %1").arg(sUnique));
    }
    else
    {
        LOG_METHOD_ERROR(QString("Snapshot dump failed for %1").arg(sUnique));
    }
}
