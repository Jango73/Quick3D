
// Qt
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif

// Application
#include "Quick3DTest.h"

// #include <vld.h>

static const char* sArg_Help	= "--help";									// Affiche l'aide
static const char* sArg_Scene	= "--scene";								// Nom de la sc�ne

static void printUsage()
{
	QTextStream sOut(stdout);

	sOut << "\n";
	sOut << "Usage:\n";
	sOut << "  Quick3DTest\n";
	sOut << "  Quick3DTest [params]\n";
	sOut << "\n";
	sOut << "params:\n";
	sOut << "  " << sArg_Scene << ": specify startup scene\n";
}

int main(int argc, char *argv[])
{
	// VLDEnable();

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
