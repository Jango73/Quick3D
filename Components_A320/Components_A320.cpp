
#include "CPluginInterface.h"

#include "Constants.h"

#include "CAirbusController.h"
#include "CAirbusStaticPort.h"
#include "CAirbusPitotPort.h"
#include "CAirbusAOASensor.h"
#include "CAirbusEngineGenerator.h"
#include "CAirbusFlightComputer.h"
#include "CAirbusADIRU.h"
#include "CAirbusDMC.h"
#include "CAirbusELAC.h"
#include "CAirbusFAC.h"
#include "CAirbusFADEC.h"
#include "CAirbusFCU.h"
#include "CAirbusFMGC.h"
#include "CAirbusSEC.h"

extern "C"
{

COMPONENTS_A320_EXPORT bool PLUGIN_ENTRY_NAME(CPluginInterface* pInterface)
{
    pInterface->registerComponent(ClassName_CAirbusController, CAirbusController::instanciator);
    pInterface->registerComponent(ClassName_CAirbusStaticPort, CAirbusStaticPort::instanciator);
    pInterface->registerComponent(ClassName_CAirbusPitotPort, CAirbusPitotPort::instanciator);
    pInterface->registerComponent(ClassName_CAirbusAOASensor, CAirbusAOASensor::instanciator);
    pInterface->registerComponent(ClassName_CAirbusEngineGenerator, CAirbusEngineGenerator::instanciator);
    pInterface->registerComponent(ClassName_CAirbusFlightComputer, CAirbusFlightComputer::instanciator);
    pInterface->registerComponent(ClassName_CAirbusADIRU, CAirbusADIRU::instanciator);
    pInterface->registerComponent(ClassName_CAirbusDMC, CAirbusDMC::instanciator);
    pInterface->registerComponent(ClassName_CAirbusELAC, CAirbusELAC::instanciator);
    pInterface->registerComponent(ClassName_CAirbusFAC, CAirbusFAC::instanciator);
    pInterface->registerComponent(ClassName_CAirbusFADEC, CAirbusFADEC::instanciator);
    pInterface->registerComponent(ClassName_CAirbusFCU, CAirbusFCU::instanciator);
    pInterface->registerComponent(ClassName_CAirbusFMGC, CAirbusFMGC::instanciator);
    pInterface->registerComponent(ClassName_CAirbusSEC, CAirbusSEC::instanciator);

    return true;
}

}
