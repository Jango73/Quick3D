
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
#include "CQuadDroneController.h"
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
#include "CEngineGenerator.h"
#include "CHydraulicGenerator.h"

#include "CHydraulicComponent.h"

//-------------------------------------------------------------------------------------------------

QMap<QString, MComponentInstantiator> CComponentFactory::s_vComponentInstantiators;

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
    registerComponent(ClassName_CComponent, CComponent::instantiator);
    registerComponent(ClassName_CPhysicalComponent, CPhysicalComponent::instantiator);
    registerComponent(ClassName_CCamera, CCamera::instantiator);
    registerComponent(ClassName_CLight, CLight::instantiator);
    registerComponent(ClassName_CStandardController, CStandardController::instantiator);
    registerComponent(ClassName_CForceController, CForceController::instantiator);
    registerComponent(ClassName_CCartoController, CCartoController::instantiator);
    registerComponent(ClassName_CTankController, CTankController::instantiator);
    registerComponent(ClassName_CAircraftController, CAircraftController::instantiator);
    registerComponent(ClassName_CQuadDroneController, CQuadDroneController::instantiator);
    registerComponent(ClassName_CBasicAnimator, CBasicAnimator::instantiator);
    registerComponent(ClassName_CSkyBox, CSkyBox::instantiator);
    registerComponent(ClassName_CWorldTerrain, CWorldTerrain::instantiator);
    registerComponent(ClassName_CVegetationGenerator, CVegetationGenerator::instantiator);

    registerComponent(ClassName_CMesh, CMesh::instantiator);
    registerComponent(ClassName_CArmature, CArmature::instantiator);
    registerComponent(ClassName_CBone, CBone::instantiator);

    registerComponent(ClassName_CVehicle, CVehicle::instantiator);
    registerComponent(ClassName_CTerrestrialVehicle, CTerrestrialVehicle::instantiator);
    registerComponent(ClassName_CSeaVehicle, CSeaVehicle::instantiator);
    registerComponent(ClassName_CWheel, CWheel::instantiator);
    registerComponent(ClassName_CMan, CMan::instantiator);

    registerComponent(ClassName_CAircraft, CAircraft::instantiator);
    registerComponent(ClassName_CWing, CWing::instantiator);
    registerComponent(ClassName_CElevator, CElevator::instantiator);
    registerComponent(ClassName_CRudder, CRudder::instantiator);
    registerComponent(ClassName_CEngine, CEngine::instantiator);
    registerComponent(ClassName_CJetEngine, CJetEngine::instantiator);

    registerComponent(ClassName_CElectricalComponent, CElectricalComponent::instantiator);
    registerComponent(ClassName_CElectricalBus, CElectricalBus::instantiator);
    registerComponent(ClassName_CElectricalContactor, CElectricalContactor::instantiator);
    registerComponent(ClassName_CElectricalConsumer, CElectricalConsumer::instantiator);
    registerComponent(ClassName_CEngineGenerator, CEngineGenerator::instantiator);
    registerComponent(ClassName_CHydraulicGenerator, CHydraulicGenerator::instantiator);

    registerComponent(ClassName_CHydraulicComponent, CHydraulicComponent::instantiator);

    CPluginLoader::getInstance()->loadPlugins();
}

//-------------------------------------------------------------------------------------------------

void CComponentFactory::registerComponent(QString sClassName, MComponentInstantiator pInstantiator)
{
    s_vComponentInstantiators[sClassName] = pInstantiator;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentFactory::instantiateComponent(QString sClassName, C3DScene* pScene)
{
    if (s_vComponentInstantiators.contains(sClassName))
    {
        return s_vComponentInstantiators[sClassName](pScene);
    }

    return nullptr;
}
