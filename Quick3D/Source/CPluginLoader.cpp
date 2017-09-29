
// qt-plus
#include "CLogger.h"

// Application
#include "CPluginLoader.h"

//-------------------------------------------------------------------------------------------------

CPluginLoader::CPluginLoader()
{
    m_sPluginPath = QCoreApplication::applicationDirPath() + "/Plugins";
}

//-------------------------------------------------------------------------------------------------

QString CPluginLoader::pluginPath() const
{
    return m_sPluginPath;
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
        loadPlugin(m_sPluginPath + "/" + sFile);
    }
}

//-------------------------------------------------------------------------------------------------

void CPluginLoader::loadPlugin(const QString& sName)
{
    QLibrary library(sName);

    if (library.load())
    {
        PluginEntry pEntry = (PluginEntry) library.resolve(STR(PLUGIN_ENTRY_NAME));

        if (pEntry != nullptr)
        {
            pEntry(CPluginInterface::getInstance());
        }
    }
    else
    {
        LOG_METHOD_CRITICAL(QString("Unable to load %1").arg(library.fileName()));
        LOG_METHOD_CRITICAL(library.errorString());
    }
}
