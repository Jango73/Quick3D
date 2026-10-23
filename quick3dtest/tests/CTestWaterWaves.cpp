
// Qt
#include <QFile>

// Application
#include "CTestWaterWaves.h"
#include "CWaterWaves.h"
#include "CWaterMaterial.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestWaterWaves::CTestWaterWaves()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Calm water has zero height and an up normal everywhere.
*/
/***************************************************************************/
void CTestWaterWaves::testCalmWaterIsFlat()
{
    QVector<Math::CVector3> vPositions;
    vPositions.append(Math::CVector3(0.0, 0.0, 0.0));
    vPositions.append(Math::CVector3(120.0, 0.0, 340.0));
    vPositions.append(Math::CVector3(-500.0, 0.0, 1000.0));

    foreach (const Math::CVector3& vPosition, vPositions)
    {
        QVERIFY(CWaterWaves::heightAt(vPosition, 10.0, 0.0) == 0.0);

        double dHeightM = 1.0;
        Math::CVector3 vNormal(1.0, 0.0, 0.0);
        CWaterWaves::surfaceAt(vPosition, 10.0, 0.0, dHeightM, vNormal);

        QVERIFY(dHeightM == 0.0);
        QVERIFY(qAbs(vNormal.X) < 1.0e-12);
        QVERIFY(qAbs(vNormal.Y - 1.0) < 1.0e-12);
        QVERIFY(qAbs(vNormal.Z) < 1.0e-12);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Wave height scales linearly with the wind level.
*/
/***************************************************************************/
void CTestWaterWaves::testWindScalesHeightLinearly()
{
    QVector<Math::CVector3> vPositions;
    vPositions.append(Math::CVector3(120.0, 0.0, 340.0));
    vPositions.append(Math::CVector3(-500.0, 0.0, 1000.0));
    vPositions.append(Math::CVector3(48.0, 0.0, 2.0));

    foreach (const Math::CVector3& vPosition, vPositions)
    {
        double dReferenceM = CWaterWaves::heightAt(vPosition, 10.0, 1.0);
        double dDoubledM = CWaterWaves::heightAt(vPosition, 10.0, 2.0);

        QVERIFY(qAbs(dReferenceM) > 1.0e-6);
        QVERIFY(qAbs(dDoubledM - 2.0 * dReferenceM) < 1.0e-9);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Wave height changes over time at a fixed position.
*/
/***************************************************************************/
void CTestWaterWaves::testTimeMovesWaves()
{
    Math::CVector3 vPosition(120.0, 0.0, 340.0);

    double dMinimumM = 1.0e30;
    double dMaximumM = -1.0e30;

    for (int iStep = 0; iStep <= 10; iStep++)
    {
        double dHeightM = CWaterWaves::heightAt(vPosition, double(iStep) * 0.5, 1.0);

        dMinimumM = qMin(dMinimumM, dHeightM);
        dMaximumM = qMax(dMaximumM, dHeightM);
    }

    QVERIFY2(dMaximumM - dMinimumM > 1.0e-3, "Waves do not move with time");
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The buoyancy plane normal is unit length and points upward.
*/
/***************************************************************************/
void CTestWaterWaves::testSurfaceNormalIsUnitAndUp()
{
    QVector<Math::CVector3> vPositions;
    vPositions.append(Math::CVector3(120.0, 0.0, 340.0));
    vPositions.append(Math::CVector3(-500.0, 0.0, 1000.0));
    vPositions.append(Math::CVector3(48.0, 0.0, 2.0));

    foreach (const Math::CVector3& vPosition, vPositions)
    {
        double dHeightM = 0.0;
        Math::CVector3 vNormal;
        CWaterWaves::surfaceAt(vPosition, 10.0, 1.0, dHeightM, vNormal);

        QVERIFY(qAbs(vNormal.magnitude() - 1.0) < 1.0e-9);
        QVERIFY2(vNormal.Y > 0.0, "Buoyancy normal leaves the up hemisphere");
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief The material query matches the canonical definition.
*/
/***************************************************************************/
void CTestWaterWaves::testMaterialDelegatesToCanonicalDefinition()
{
    C3DScene cScene(false);
    cScene.setTime(12.5);
    cScene.setWindLevel(1.0);

    CWaterMaterial cMaterial(&cScene);

    CGeoloc gPosition(48.0, 2.0, 0.0);

    double dExpectedM = CWaterWaves::heightAt(gPosition.toVector3() - cScene.worldOrigin(), cScene.time(), cScene.windLevel());

    QVERIFY(cMaterial.getHeightAt(gPosition) == dExpectedM);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Both vertex shaders carry the canonical wave constants.
*/
/***************************************************************************/
void CTestWaterWaves::testShaderMirrorsCanonicalConstants()
{
    QStringList vPaths;
    vPaths.append(QStringLiteral(":/resources/shaders/glsl1/VS_Standard.c"));
    vPaths.append(QStringLiteral(":/resources/shaders/glsl4/VS_Standard.c"));

    QStringList vConstants;
    vConstants.append(QStringLiteral("* 0.005"));
    vConstants.append(QStringLiteral("* 0.1"));
    vConstants.append(QStringLiteral("vec3(2.0, 2.0, 2.0)"));
    vConstants.append(QStringLiteral("u_wave_amplitude * 4.0"));
    vConstants.append(QStringLiteral("127.1, 311.7, 321.4"));
    vConstants.append(QStringLiteral("43758.5453123"));

    foreach (const QString& sPath, vPaths)
    {
        QFile cFile(sPath);

        QVERIFY2(cFile.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(QString("Missing shader %1").arg(sPath)));

        QString sSource = QString::fromUtf8(cFile.readAll());

        foreach (const QString& sConstant, vConstants)
        {
            QVERIFY2(sSource.contains(sConstant), qPrintable(QString("%1 lost %2").arg(sPath, sConstant)));
        }
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief A fixed sample locks the Perlin implementation against silent drift.
*/
/***************************************************************************/
void CTestWaterWaves::testHeightRegressionAnchor()
{
    double dHeightM = CWaterWaves::heightAt(Math::CVector3(120.0, 0.0, 340.0), 10.0, 1.0);

    QVERIFY2(qAbs(dHeightM - 5.857505945404558) < 1.0e-6, qPrintable(QString("Wave anchor drifted: %1").arg(dHeightM, 0, 'g', 17)));
}
