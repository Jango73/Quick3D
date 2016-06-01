
// Application
#include "CPreferencesManager.h"

//-------------------------------------------------------------------------------------------------

CPreferencesManager::CPreferencesManager()
{
	QString sFileName = QCoreApplication::applicationDirPath() + "/" + "Preferences.xml";

	xPreferences = CXMLNode::loadFromFile(sFileName);
}

//-------------------------------------------------------------------------------------------------

CPreferencesManager::~CPreferencesManager()
{
}
