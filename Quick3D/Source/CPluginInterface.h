
#pragma once

// Application
#include "quick3d_global.h"
#include "CSingleton.h"
#include "ComponentInstantiator.h"

//-------------------------------------------------------------------------------------------------
// Constants

#define PLUGIN_ENTRY_NAME	Quick3DPluginEntry

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPluginInterface : public CSingleton<CPluginInterface>
{
    friend class CSingleton<CPluginInterface>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CPluginInterface();

    //!
    void registerComponent(QString sClassName, MComponentInstantiator pInstantiator);
};

//-------------------------------------------------------------------------------------------------

extern "C"
{
typedef bool (*PluginEntry)(CPluginInterface*);
}
