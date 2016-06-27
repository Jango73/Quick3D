
// Application
#include "CPluginLoader.h"

//-------------------------------------------------------------------------------------------------

CPluginLoader::CPluginLoader()
{
    m_sPluginPath = QCoreApplication::applicationDirPath() + "/Plugins";
}

//-------------------------------------------------------------------------------------------------

#define XSTR(x) #x
#define STR(x) XSTR(x)

void CPluginLoader::loadPlugins()
{
    QStringList slNameFilter;

    slNameFilter.append(QString("*.dll"));

    QDir dDirectory(m_sPluginPath);
    QStringList lFiles = dDirectory.entryList(slNameFilter);

    foreach (QString sFile, lFiles)
    {
        QLibrary library(m_sPluginPath + "/" + sFile);

        if (library.load())
        {
            PluginEntry pEntry = (PluginEntry) library.resolve(STR(PLUGIN_ENTRY_NAME));

            if (pEntry != NULL)
            {
                pEntry(CPluginInterface::getInstance());
            }
        }
    }
}
