
// Qt
#include <QFile>

// Quick3D
#include "CQ3DConstants.h"
#include "CGeoTree.h"
#include "CWaypoint.h"

// Application
#include "CTestGeo.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestGeo::CTestGeo()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Query around France finds the two French waypoints.
*/
/***************************************************************************/
void CTestGeo::testQueryFindsNearbyWaypoints()
{
    CWaypoint* pNorth = new CWaypoint(wtVORDME, "WP1", CGeoloc(48.999472, 2.623806, 0.0), 0.0);
    CWaypoint* pCenter = new CWaypoint(wtVORDME, "WP2", CGeoloc(45.600000, 3.400000, 0.0), 0.0);
    CWaypoint* pAfrica = new CWaypoint(wtVORDME, "WP3", CGeoloc(32.500000, 20.200000, 0.0), 0.0);
    CWaypoint* pAsia = new CWaypoint(wtVORDME, "WP4", CGeoloc(5.500000, 80.400000, 0.0), 0.0);

    CGeoTree tTree(2);
    tTree.append(pNorth);
    tTree.append(pCenter);
    tTree.append(pAfrica);
    tTree.append(pAsia);

    QVector<CGeolocalized*> vResult = tTree.query(CGeoloc(48.5, 2.2, 0.0), 400000.0);

    QCOMPARE(vResult.count(), 2);
    QVERIFY(vResult.contains(pNorth));
    QVERIFY(vResult.contains(pCenter));

    delete pNorth;
    delete pCenter;
    delete pAfrica;
    delete pAsia;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Query far from every waypoint returns nothing.
*/
/***************************************************************************/
void CTestGeo::testQueryIgnoresFarWaypoints()
{
    CWaypoint* pNorth = new CWaypoint(wtVORDME, "WP1", CGeoloc(48.999472, 2.623806, 0.0), 0.0);
    CWaypoint* pCenter = new CWaypoint(wtVORDME, "WP2", CGeoloc(45.600000, 3.400000, 0.0), 0.0);

    CGeoTree tTree(2);
    tTree.append(pNorth);
    tTree.append(pCenter);

    QVector<CGeolocalized*> vResult = tTree.query(CGeoloc(0.0, 0.0, 0.0), 1000.0);

    QVERIFY(vResult.isEmpty());

    delete pNorth;
    delete pCenter;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Dump writes a non empty file in the isolated test folder.
*/
/***************************************************************************/
void CTestGeo::testDumpCreatesFile()
{
    CWaypoint* pNorth = new CWaypoint(wtVORDME, "WP1", CGeoloc(48.999472, 2.623806, 0.0), 0.0);

    CGeoTree tTree(2);
    tTree.append(pNorth);

    QFile tDump("Geotree.dump.txt");

    QVERIFY(tDump.open(QIODevice::WriteOnly));

    QTextStream sStream(&tDump);
    tTree.dump(sStream, 0);
    tDump.close();

    QVERIFY(tDump.exists());
    QVERIFY(tDump.size() > 0);

    delete pNorth;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Geoloc built from a reference and a local vector is deterministic.
*/
/***************************************************************************/
void CTestGeo::testGeolocOffsetIsDeterministic()
{
    double dFactor = FAC_NM_TO_M;

    CGeoloc gReference(48.723333, 2.379444, 0);
    CVector3 vOffset(0.0, 0.0, -23.0 * dFactor);

    CGeoloc gFirst(gReference, vOffset);
    CGeoloc gSecond(gReference, vOffset);

    QCOMPARE(gSecond.toString(), gFirst.toString());
    QVERIFY(gFirst.toString() != gReference.toString());
}

//-------------------------------------------------------------------------------------------------
