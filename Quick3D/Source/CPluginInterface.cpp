
// Application
#include "CPluginInterface.h"
#include "CComponentFactory.h"

//-------------------------------------------------------------------------------------------------

CPluginInterface::CPluginInterface()
{
}

//-------------------------------------------------------------------------------------------------

void CPluginInterface::registerComponent(QString sClassName, MComponentInstantiator pInstantiator)
{
    CComponentFactory::getInstance()->registerComponent(sClassName, pInstantiator);
}
