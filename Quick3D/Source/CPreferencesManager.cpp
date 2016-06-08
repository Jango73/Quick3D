
// Application
#include "CPreferencesManager.h"

//-------------------------------------------------------------------------------------------------

CPreferencesManager::CPreferencesManager()
{
	QString sFileName = QCoreApplication::applicationDirPath() + "/" + "Preferences.xml";

    xPreferences = CXMLNode::loadXMLFromFile(sFileName);
}

//-------------------------------------------------------------------------------------------------

CPreferencesManager::~CPreferencesManager()
{
}
