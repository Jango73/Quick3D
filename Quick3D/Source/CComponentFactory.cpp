
// Application
#include "CQ3DConstants.h"
#include "CRessourcesManager.h"
#include "CWorkerManager.h"
#include "CComponentLoader.h"
#include "CBuildingGenerator.h"
#include "CTreeGenerator.h"
#include "CPerlin.h"
#include "COBJLoader.h"
#include "CComponentFactory.h"
#include "CPluginLoader.h"

#include "CComponent.h"
#include "CPhysicalComponent.h"
#include "CCamera.h"
#include "CLight.h"
#include "CStandardController.h"
#include "CForceController.h"
#include "CCartoController.h"
#include "CTankController.h"
#include "CAircraftController.h"
#include "CBasicAnimator.h"
#include "CSkyBox.h"
#include "CWorldTerrain.h"
#include "CVegetationGenerator.h"
#include "CMesh.h"
#include "CArmature.h"
#include "CBone.h"

#include "CVehicle.h"
#include "CTerrestrialVehicle.h"
#include "CSeaVehicle.h"
#include "CWheel.h"
#include "CMan.h"

#include "CAircraft.h"
#include "CWing.h"
#include "CElevator.h"
#include "CRudder.h"
#include "CEngine.h"
#include "CJetEngine.h"

#include "CElectricalComponent.h"
#include "CElectricalBus.h"
#include "CElectricalContactor.h"
#include "CElectricalConsumer.h"

#include "CHydraulicComponent.h"

//-------------------------------------------------------------------------------------------------

QMap<QString, MComponentInstanciator> CComponentFactory::s_vComponentInstanciators;

//-------------------------------------------------------------------------------------------------

CComponentFactory::CComponentFactory()
{
}

//-------------------------------------------------------------------------------------------------

CComponentFactory::~CComponentFactory()
{
    CWorkerManager::killInstance();
    CComponentLoader::killInstance();
    COBJLoader::killInstance();
    CPerlin::killInstance();
}

//-------------------------------------------------------------------------------------------------

void CComponentFactory::registerCoreComponents()
{
    registerComponent(ClassName_CComponent, CComponent::instanciator);
    registerComponent(ClassName_CPhysicalComponent, CPhysicalComponent::instanciator);
    registerComponent(ClassName_CCamera, CCamera::instanciator);
    registerComponent(ClassName_CLight, CLight::instanciator);
    registerComponent(ClassName_CStandardController, CStandardController::instanciator);
    registerComponent(ClassName_CForceController, CForceController::instanciator);
    registerComponent(ClassName_CCartoController, CCartoController::instanciator);
    registerComponent(ClassName_CTankController, CTankController::instanciator);
    registerComponent(ClassName_CAircraftController, CAircraftController::instanciator);
    registerComponent(ClassName_CBasicAnimator, CBasicAnimator::instanciator);
    registerComponent(ClassName_CSkyBox, CSkyBox::instanciator);
    registerComponent(ClassName_CWorldTerrain, CWorldTerrain::instanciator);
    registerComponent(ClassName_CVegetationGenerator, CVegetationGenerator::instanciator);

    registerComponent(ClassName_CMesh, CMesh::instanciator);
    registerComponent(ClassName_CArmature, CArmature::instanciator);
    registerComponent(ClassName_CBone, CBone::instanciator);

    registerComponent(ClassName_CVehicle, CVehicle::instanciator);
    registerComponent(ClassName_CTerrestrialVehicle, CTerrestrialVehicle::instanciator);
    registerComponent(ClassName_CSeaVehicle, CSeaVehicle::instanciator);
    registerComponent(ClassName_CWheel, CWheel::instanciator);
    registerComponent(ClassName_CMan, CMan::instanciator);

    registerComponent(ClassName_CAircraft, CAircraft::instanciator);
    registerComponent(ClassName_CWing, CWing::instanciator);
    registerComponent(ClassName_CElevator, CElevator::instanciator);
    registerComponent(ClassName_CRudder, CRudder::instanciator);
    registerComponent(ClassName_CEngine, CEngine::instanciator);
    registerComponent(ClassName_CJetEngine, CJetEngine::instanciator);

    registerComponent(ClassName_CElectricalComponent, CElectricalComponent::instanciator);
    registerComponent(ClassName_CElectricalBus, CElectricalBus::instanciator);
    registerComponent(ClassName_CElectricalContactor, CElectricalContactor::instanciator);
    registerComponent(ClassName_CElectricalConsumer, CElectricalConsumer::instanciator);

    registerComponent(ClassName_CHydraulicComponent, CHydraulicComponent::instanciator);

    CPluginLoader::getInstance()->loadPlugins();
}

//-------------------------------------------------------------------------------------------------

void CComponentFactory::registerComponent(QString sClassName, MComponentInstanciator pInstanciator)
{
    s_vComponentInstanciators[sClassName] = pInstanciator;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentFactory::instanciateComponent(QString sClassName, C3DScene* pScene)
{
    if (s_vComponentInstanciators.contains(sClassName))
    {
        return s_vComponentInstanciators[sClassName](pScene);
    }

    return nullptr;
}
