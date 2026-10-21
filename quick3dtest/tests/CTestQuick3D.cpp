
// Application
#include "CTestQuick3D.h"
#include "CTestMath.h"
#include "CTestCamera.h"
#include "CTestGeo.h"
#include "CTestPhysics.h"
#include "CTestVehicle.h"
#include "CTestScene.h"
#include "CTestTerrainSkirt.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor.
*/
/***************************************************************************/
CTestQuick3D::CTestQuick3D()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Runs every test suite sequentially.
* \return Combined status, 0 when every suite passes.
*/
/***************************************************************************/
int CTestQuick3D::runAllTests()
{
    int iStatus = 0;

    {
        CTestMath oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestCamera oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestGeo oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestPhysics oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestVehicle oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestScene oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    {
        CTestTerrainSkirt oTest;
        iStatus |= QTest::qExec(&oTest);
    }

    return iStatus;
}

//-------------------------------------------------------------------------------------------------
