
// Qt
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif

// #define USE_VLD

// Application
#include "Quick3DTest.h"
#include "CUnitTests.h"

#ifdef USE_VLD
#include <vld.h>
#endif

static const char* sArg_Help        = "--help";         // Affiche l'aide
static const char* sArg_Scene       = "--scene";        // Nom de la scène
static const char* sArg_UnitTests   = "--unit-tests";   // Unit tests mode

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
    sOut << "  " << sArg_Scene << ": specify startup scene\n";
    sOut << "  " << sArg_UnitTests << ": runs unit tests\n";
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
    else if (lArgList.contains(sArg_UnitTests))
    {
        CUnitTests tests;
        tests.run();
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
        Quick3DTest w(sSceneFileName);
        w.show();
        return a.exec();
    }

    return 0;
}
