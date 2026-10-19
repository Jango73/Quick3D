
// Qt
#include <QCoreApplication>
#include <QDir>

// Application
#include "CTestFixture.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Constructor, points the fixture at the isolated test folder.
*/
/***************************************************************************/
CTestFixture::CTestFixture()
{
    // Tests manipulate and delete files: run them in an isolated folder
    // under <repository>/tmp, never in the current working folder
    m_sRootPath = testDirectoryPath();
    QDir::setCurrent(m_sRootPath);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
* \brief Returns the isolated test folder, created if needed.
* \return The <repository>/tmp/quick3d-tests folder path.
*/
/***************************************************************************/
QString CTestFixture::testDirectoryPath()
{
    // Locate the repository root from the binary folder, then use tmp/
    QDir tFolder(QCoreApplication::applicationDirPath());
    while (not tFolder.exists("quick3dtest/Quick3DTest.pro") and tFolder.cdUp())
    {
    }

    if (not tFolder.exists("quick3dtest/Quick3DTest.pro"))
        qFatal("CTestFixture: repository root (quick3dtest/Quick3DTest.pro) not found, tests refuse to run outside a repository checkout");

    QString sTestPath = QString("%1/tmp/quick3d-tests").arg(tFolder.absolutePath());
    QDir().mkpath(sTestPath);

    return sTestPath;
}

//-------------------------------------------------------------------------------------------------
