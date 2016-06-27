
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

#include "CSingleton.h"
#include "ComponentInstanciator.h"

//-------------------------------------------------------------------------------------------------

#define PLUGIN_ENTRY_NAME	Quick3DPluginEntry

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CPluginInterface : public CSingleton<CPluginInterface>
{
    friend class CSingleton<CPluginInterface>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par défaut
    CPluginInterface();

    //!
    void registerComponent(QString sClassName, MComponentInstanciator pInstanciator);
};

//-------------------------------------------------------------------------------------------------

extern "C"
{
typedef bool (*PluginEntry)(CPluginInterface*);
}
