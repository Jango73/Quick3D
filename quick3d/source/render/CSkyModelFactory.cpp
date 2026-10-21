
// Application
#include "CSkyModelFactory.h"
#include "CSkyModel.h"
#include "CRayleighSkyModel.h"
#include "CEnvironmentDefaults.h"

//-------------------------------------------------------------------------------------------------

/*!
    Creates the model matching \a sName, null when unknown.
*/
QSP<CSkyModel> CSkyModelFactory::create(const QString& sName)
{
    if (sName == SkyModels::PhysicalName)
    {
        return QSP<CSkyModel>(new CRayleighSkyModel());
    }

    return QSP<CSkyModel>(nullptr);
}
