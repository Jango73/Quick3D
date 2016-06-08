
// Qt
#include <QTextStream>

// Fondations
#include "CLogManager.h"

// Application
#include "C3DScene.h"
#include "CComponentLoader.h"
#include "CComponentFactory.h"
#include "CAircraftController.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponentLoader::CComponentLoader()
{
    LOG_DEBUG("CComponentLoader::CComponentLoader()");
}

//-------------------------------------------------------------------------------------------------

QVector<CComponent*> CComponentLoader::load(C3DScene* pScene, QString sFileName)
{
    LOG_DEBUG("CComponentLoader::load()");

    QVector<CComponent*> vOutput;

    CXMLNode tDoc = CXMLNode::loadXMLFromFile(sFileName);

    QString sCamera1;
    QString sCamera2;
    QString sControlled;

    foreach (CXMLNode xNode, tDoc.nodes())
    {
        if (xNode.tag() == "Camera1")
        {
            sCamera1 = xNode.attributes()[ParamName_Name];
        }
        else if (xNode.tag() == "Camera2")
        {
            sCamera2 = xNode.attributes()[ParamName_Name];
        }
        else if (xNode.tag() == "ControlledComponent")
        {
            sControlled = xNode.attributes()[ParamName_Name];
        }
        else if (xNode.tag() == ParamName_Components)
        {
            QVector<CXMLNode> xComponents = xNode.getNodesByTagName(ParamName_Component);

            foreach (CXMLNode xComponent, xComponents)
            {
                CComponent* pComponent = loadComponent(pScene, xComponent, NULL);

                if (pComponent != NULL)
                {
                    vOutput.append(pComponent);
                }
            }
        }
    }

    foreach (CComponent* pComponent, vOutput)
    {
        CComponent* pCamera1Found = pComponent->findComponent(sCamera1);
        CComponent* pCamera2Found = pComponent->findComponent(sCamera2);
        CComponent* pControlledFound = pComponent->findComponent(sControlled);

        if (pCamera1Found != NULL && pCamera1Found->isCamera())
        {
            if (pScene->getViewports().contains(0))
            {
                pScene->getViewports()[0]->setCamera(dynamic_cast<CCamera*>(pCamera1Found));
            }
        }

        if (pCamera2Found != NULL && pCamera2Found->isCamera())
        {
            if (pScene->getViewports().contains(1))
            {
                pScene->getViewports()[1]->setCamera(dynamic_cast<CCamera*>(pCamera2Found));
            }
        }

        if (pControlledFound != NULL && pControlledFound->getController() != NULL)
        {
            pScene->setController(pControlledFound->getController());
        }
    }

    LOG_INFO("CComponentLoader::load() : Finished loading scene");

    return vOutput;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentLoader::loadComponent(C3DScene* pScene, QString sFileName)
{
    CXMLNode xComponent = CXMLNode::loadXMLFromFile(sFileName);

    CComponent* pNewComponent = loadComponent(pScene, xComponent, NULL);

    return pNewComponent;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentLoader::loadComponent(C3DScene* pScene, CXMLNode xComponent, CComponent* pParent)
{
    QString sClass = xComponent.attributes()[ParamName_Class];

    CComponent* pComponent = CComponentFactory::getInstance()->instanciateComponent(sClass, pScene);

    if (pComponent != NULL)
    {
        if (pParent != NULL)
        {
            pComponent->setParentName(pParent->getName());
        }

        pComponent->loadParameters(xComponent);

        LOG_DEBUG(QString("... Adding component %1").arg(pComponent->getName()));

        // Chargement des composants enfants

        QVector<CXMLNode> xChildComponents = xComponent.getNodesByTagName(ParamName_Component);

        foreach (CXMLNode xChildComponent, xChildComponents)
        {
            CComponent* pChildComponent = loadComponent(pScene, xChildComponent, pComponent);

            if (pChildComponent != NULL)
            {
                pChildComponent->setParent(pComponent);
            }
        }

        // Chargement du contrôleur

        CXMLNode xController = xComponent.getNodeByTagName(ParamName_Controller);

        if (xController.isEmpty() == false)
        {
            QString sControllerClass = xController.attributes()[ParamName_Class];

            CComponent* pControllerComponent = CComponentFactory::getInstance()->instanciateComponent(sControllerClass, pScene);

            if (pControllerComponent != NULL)
            {
                pControllerComponent->loadParameters(xController);

                pComponent->setController(dynamic_cast<CController*>(pControllerComponent));
                pControllerComponent->setParent(pComponent);
            }
        }

        return pComponent;
    }

    return NULL;
}
