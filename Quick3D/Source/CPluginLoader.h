
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#include "CSingleton.h"
#include "CPluginInterface.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPluginLoader : public CSingleton<CPluginLoader>
{
    friend class CSingleton<CPluginLoader>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par défaut
    CPluginLoader();

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    void loadPlugins();

protected:

    QString	m_sPluginPath;
};
