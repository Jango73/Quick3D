
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
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CPluginLoader();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    void loadPlugins();

protected:

    QString	m_sPluginPath;
};
