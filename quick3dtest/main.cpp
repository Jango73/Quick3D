
// Qt
#include <QtGlobal>
#include <QtTest/QtTest>

#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif

// #define USE_VLD

// Application
#include "Quick3DTest.h"
#include "tests/CTestQuick3D.h"

// qt-plus
#include "CTimeSampler.h"

#ifdef USE_VLD
#include <vld.h>
#endif

static const char* sArg_Help        = "--help";         // Shows help
static const char* sArg_Scene       = "--scene";        // Scene name
static const char* sArg_RunTests    = "run-tests";      // Unit tests mode, like aze
static const char* sArg_CheckRender = "--check-render"; // Capture view, log black pixel ratio, quit
static const char* sArg_Sample      = "--sample";        // Enable CTimeSampler dumps (off by default)

static void printUsage()
{
    QTextStream sOut(stdout);

    sOut << "\n";
    sOut << "Usage:\n";
    sOut << "  Quick3DTest\n";
    sOut << "  Quick3DTest [params]\n";
    sOut << "\n";
    sOut << "params:\n";
    sOut << "  " << sArg_Help << ": shows this help\n";
    sOut <<   "  " << sArg_Scene << ": specify startup scene\n";
    sOut << "  " << sArg_RunTests << ": runs unit tests\n";
     sOut << "  " << sArg_CheckRender << ": captures the 3D view, logs the black pixel ratio, quits\n";
     sOut << "  " << sArg_Sample << ": enables CTimeSampler timing dumps\n";
}

int main(int argc, char *argv[])
{
#ifdef USE_VLD
    VLDEnable();
#endif

    QStringList lArgList;
    QString sSceneFileName = "Quick3DTest.xml";

    if (argc > 1)
    {
        for (int Index = 0; Index < argc; Index++) lArgList.append(QString(argv[Index]));
    }

    if (lArgList.contains(sArg_Help))
    {
        printUsage();
    }
    else if (lArgList.contains(sArg_RunTests))
    {
        QCoreApplication tApplication(argc, argv);
        CTimeSampler::getInstance()->setEnabled(lArgList.contains(sArg_Sample));
        CTestQuick3D tests;
        QTEST_SET_MAIN_SOURCE_PATH
        return tests.runAllTests();
    }
    else
    {
        if (lArgList.contains(sArg_Scene))
        {
            int iIndexArg_Scene = lArgList.indexOf(sArg_Scene) + 1;

            if (iIndexArg_Scene > 0)
            {
                sSceneFileName = lArgList[iIndexArg_Scene];
            }
        }

        QApplication a(argc, argv);
        CTimeSampler::getInstance()->setEnabled(lArgList.contains(sArg_Sample));
        Quick3DTest w(sSceneFileName);
        w.setCheckRenderMode(lArgList.contains(sArg_CheckRender));
        w.show();
        return a.exec();
    }

    return 0;
}
