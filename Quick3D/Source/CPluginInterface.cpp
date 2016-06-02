
// Application
#include "CPluginInterface.h"
#include "CComponentFactory.h"

//-------------------------------------------------------------------------------------------------

CPluginInterface::CPluginInterface()
{
}

//-------------------------------------------------------------------------------------------------

void CPluginInterface::registerComponent(QString sClassName, MComponentInstanciator pInstanciator)
{
	CComponentFactory::getInstance()->registerComponent(sClassName, pInstanciator);
}
