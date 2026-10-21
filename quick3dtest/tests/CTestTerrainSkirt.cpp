
// Application
#include "CTestTerrainSkirt.h"
#include "CMeshGeometry.h"
#include "CPatchSkirtBuilder.h"
#include "CVertex.h"
#include "CFace.h"

//-------------------------------------------------------------------------------------------------
// Test data, expected counts derive from the grid size

#define TEST_GRID_POINTS          5
#define TEST_BORDER_COUNT         (4 * (TEST_GRID_POINTS - 1))
#define TEST_CORE_VERTEX_COUNT    (TEST_GRID_POINTS * TEST_GRID_POINTS)
#define TEST_CORE_FACE_COUNT      ((TEST_GRID_POINTS - 1) * (TEST_GRID_POINTS - 1))
#define TEST_CELL_SIZE_M          10.0
#define TEST_RELIEF_M             4.0
#define TEST_NORMAL_TOLERANCE     0.000000001
#define TEST_HUGE_INPUT_M         1000000000.0

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestTerrainSkirt::CTestTerrainSkirt()
    : CTestFixture()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Border loop of a 5 by 5 grid holds 16 unique ordered indices.
*/
/***************************************************************************/
void CTestTerrainSkirt::testBorderLoopIsClosed()
{
    QVector<int> vLoop = CPatchSkirtBuilder::borderLoop(TEST_GRID_POINTS);

    QCOMPARE(vLoop.count(), TEST_BORDER_COUNT);

    QSet<int> sUnique(vLoop.begin(), vLoop.end());

    QCOMPARE(sUnique.count(), TEST_BORDER_COUNT);

    foreach (int iIndex, vLoop)
    {
        QVERIFY(iIndex >= 0 && iIndex < TEST_CORE_VERTEX_COUNT);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Skirt depth follows cell size and relief, clamped to minimum and maximum.
*/
/***************************************************************************/
void CTestTerrainSkirt::testSkirtDepthClamps()
{
    SPatchSkirtParameters oParameters;

    double dNominal = CPatchSkirtBuilder::skirtDepth(TEST_CELL_SIZE_M, TEST_RELIEF_M, oParameters);

    QCOMPARE(dNominal, SKIRT_DEPTH_FACTOR * TEST_CELL_SIZE_M + SKIRT_RELIEF_FACTOR * TEST_RELIEF_M);

    QCOMPARE(CPatchSkirtBuilder::skirtDepth(0.0, 0.0, oParameters), oParameters.MinimumDepthMeters);
    QCOMPARE(CPatchSkirtBuilder::skirtDepth(TEST_HUGE_INPUT_M, TEST_HUGE_INPUT_M, oParameters), oParameters.MaximumDepthMeters);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Disabled parameters append no vertex and no face.
*/
/***************************************************************************/
void CTestTerrainSkirt::testDisabledAddsNothing()
{
    CMeshGeometry oMesh(nullptr);

    oMesh.createQuadPatch(TEST_GRID_POINTS, 0);

    SPatchSkirtParameters oParameters;
    oParameters.Enabled = false;

    SPatchSkirtRange oRange = CPatchSkirtBuilder::buildSkirt(&oMesh, TEST_GRID_POINTS, oParameters);

    QVERIFY(oRange.hasSkirt() == false);
    QCOMPARE(oMesh.vertices().count(), TEST_CORE_VERTEX_COUNT);
    QCOMPARE(oMesh.faces().count(), TEST_CORE_FACE_COUNT);
    QCOMPARE(oMesh.coreVertexCount(), -1);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Enabled build appends one tucked ring and records the core count.
*/
/***************************************************************************/
void CTestTerrainSkirt::testBuildSkirtAppendsTuckedRing()
{
    CMeshGeometry oMesh(nullptr);

    oMesh.createQuadPatch(TEST_GRID_POINTS, 0);

    for (int iIndex = 0; iIndex < oMesh.vertices().count(); iIndex++)
    {
        oMesh.vertices()[iIndex].gravity() = Math::CVector3(0.0, -1.0, 0.0);
        oMesh.vertices()[iIndex].altitude() = 0.0;
    }

    SPatchSkirtParameters oParameters;

    SPatchSkirtRange oRange = CPatchSkirtBuilder::buildSkirt(&oMesh, TEST_GRID_POINTS, oParameters);

    QVERIFY(oRange.hasSkirt());
    QCOMPARE(oRange.CoreVertexCount, TEST_CORE_VERTEX_COUNT);
    QCOMPARE(oRange.SkirtVertexCount, TEST_BORDER_COUNT);
    QCOMPARE(oRange.SkirtFaceCount, TEST_BORDER_COUNT);
    QCOMPARE(oMesh.vertices().count(), TEST_CORE_VERTEX_COUNT + TEST_BORDER_COUNT);
    QCOMPARE(oMesh.faces().count(), TEST_CORE_FACE_COUNT + TEST_BORDER_COUNT);
    QCOMPARE(oMesh.coreVertexCount(), TEST_CORE_VERTEX_COUNT);

    // The skirt drops below the border and tucks toward the patch center
    for (int iLoop = 0; iLoop < TEST_BORDER_COUNT; iLoop++)
    {
        int iTop = CPatchSkirtBuilder::borderLoop(TEST_GRID_POINTS)[iLoop];
        int iBottom = oRange.SkirtFirstVertex + iLoop;

        QVERIFY(oMesh.vertices()[iBottom].position().Y < oMesh.vertices()[iTop].position().Y);

        double dTopRadius = Math::CVector3(
            oMesh.vertices()[iTop].position().X, 0.0, oMesh.vertices()[iTop].position().Z).magnitude();
        double dBottomRadius = Math::CVector3(
            oMesh.vertices()[iBottom].position().X, 0.0, oMesh.vertices()[iBottom].position().Z).magnitude();

        QVERIFY(dBottomRadius < dTopRadius);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Skirt faces reference extension vertices so ray queries skip them.
*/
/***************************************************************************/
void CTestTerrainSkirt::testSkirtFacesReferenceExtensionOnly()
{
    CMeshGeometry oMesh(nullptr);

    oMesh.createQuadPatch(TEST_GRID_POINTS, 0);

    for (int iIndex = 0; iIndex < oMesh.vertices().count(); iIndex++)
    {
        oMesh.vertices()[iIndex].gravity() = Math::CVector3(0.0, -1.0, 0.0);
        oMesh.vertices()[iIndex].altitude() = 0.0;
    }

    SPatchSkirtParameters oParameters;

    SPatchSkirtRange oRange = CPatchSkirtBuilder::buildSkirt(&oMesh, TEST_GRID_POINTS, oParameters);

    QVERIFY(oRange.hasSkirt());

    for (int iFace = oRange.SkirtFirstFace; iFace < oMesh.faces().count(); iFace++)
    {
        bool bTouchesCoreOnly = true;

        foreach (int iVertexIndex, oMesh.faces()[iFace].indices())
        {
            if (iVertexIndex >= oRange.CoreVertexCount)
            {
                bTouchesCoreOnly = false;
            }
        }

        QVERIFY(bTouchesCoreOnly == false);
    }
}

/***************************************************************************/

/*!
* \brief Core border normals are unchanged by the skirt, skirt walls face outward.
*/
/***************************************************************************/
void CTestTerrainSkirt::testSkirtLeavesCoreNormalsUntouched()
{
    CMeshGeometry oMesh(nullptr);

    oMesh.createQuadPatch(TEST_GRID_POINTS, 0);
    oMesh.computeNormals();

    QVector<int> vLoop = CPatchSkirtBuilder::borderLoop(TEST_GRID_POINTS);
    QVector<Math::CVector3> vBorderNormals;

    foreach (int iIndex, vLoop)
    {
        oMesh.vertices()[iIndex].gravity() = Math::CVector3(0.0, -1.0, 0.0);
        oMesh.vertices()[iIndex].altitude() = 0.0;
        vBorderNormals.append(oMesh.vertices()[iIndex].normal());
    }

    SPatchSkirtParameters oParameters;

    SPatchSkirtRange oRange = CPatchSkirtBuilder::buildSkirt(&oMesh, TEST_GRID_POINTS, oParameters);

    QVERIFY(oRange.hasSkirt());

    oMesh.computeNormals();

    for (int iLoop = 0; iLoop < vLoop.count(); iLoop++)
    {
        Math::CVector3 vBefore = vBorderNormals[iLoop];
        Math::CVector3 vAfter = oMesh.vertices()[vLoop[iLoop]].normal();

        QVERIFY(fabs(vAfter.X - vBefore.X) < TEST_NORMAL_TOLERANCE);
        QVERIFY(fabs(vAfter.Y - vBefore.Y) < TEST_NORMAL_TOLERANCE);
        QVERIFY(fabs(vAfter.Z - vBefore.Z) < TEST_NORMAL_TOLERANCE);
    }

    Math::CVector3 vWall = oMesh.vertices()[oRange.SkirtFirstVertex].normal();

    QVERIFY(fabs(vWall.X) + fabs(vWall.Z) > MESH_NULL_VECTOR_MAGNITUDE);
}

//-------------------------------------------------------------------------------------------------
