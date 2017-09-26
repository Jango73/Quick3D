
// Application
#include "CPluginInterface.h"
#include "CComponentFactory.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CPluginInterface
    \brief Defines a bridge between Quick3D and its plugins.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CPluginInterface with its default parameters.
*/
CPluginInterface::CPluginInterface()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers the component \a pInstantiator for \a sClassName.
*/
void CPluginInterface::registerComponent(QString sClassName, MComponentInstantiator pInstantiator)
{
    CComponentFactory::getInstance()->registerComponent(sClassName, pInstantiator);
}
