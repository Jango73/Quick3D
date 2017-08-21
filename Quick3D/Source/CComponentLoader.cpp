
// Qt
#include <QTextStream>

// qt-plus
#include "CLogger.h"

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

QVector<QSP<CComponent> > CComponentLoader::load(const QString& sBaseFile, C3DScene* pScene)
{
    LOG_DEBUG("CComponentLoader::load()");

    QVector<QSP<CComponent> > vOutput;

    CXMLNode tDoc = CXMLNode::loadXMLFromFile(sBaseFile);

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
                CComponent* pComponent = loadComponent(sBaseFile, pScene, xComponent, nullptr);

                if (pComponent != nullptr)
                {
                    vOutput.append(QSP<CComponent>(pComponent));
                }
            }
        }
    }

    foreach (QSP<CComponent> pComponent, vOutput)
    {
        QSP<CComponent> pCamera1Found = pComponent->findComponent(sCamera1);
        QSP<CComponent> pCamera2Found = pComponent->findComponent(sCamera2);
        QSP<CComponent> pControlledFound = pComponent->findComponent(sControlled);

        if (pCamera1Found && pCamera1Found->isCamera())
        {
            if (pScene->viewports().contains(0))
            {
                pScene->viewports()[0]->setCamera(QSP_CAST(CCamera, pCamera1Found));
            }
        }

        if (pCamera2Found && pCamera2Found->isCamera())
        {
            if (pScene->viewports().contains(1))
            {
                pScene->viewports()[1]->setCamera(QSP_CAST(CCamera, pCamera2Found));
            }
        }

        if (pControlledFound && pControlledFound->controller() != nullptr)
        {
            pScene->setController(pControlledFound->controller());
        }
    }

    LOG_INFO("CComponentLoader::load() : Finished loading scene");

    return vOutput;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentLoader::loadComponent(const QString& sBaseFile, C3DScene* pScene)
{
    CXMLNode xComponent = CXMLNode::loadXMLFromFile(sBaseFile);

    CComponent* pNewComponent = loadComponent(sBaseFile, pScene, xComponent, nullptr);

    return pNewComponent;
}

//-------------------------------------------------------------------------------------------------

CComponent* CComponentLoader::loadComponent(const QString& sBaseFile, C3DScene* pScene, CXMLNode xComponent, CComponent* pParent)
{
    QString sClass = xComponent.attributes()[ParamName_Class];

    CComponent* pComponent = CComponentFactory::getInstance()->instanciateComponent(sClass, pScene);

    if (pComponent != nullptr)
    {
        if (pParent != nullptr)
        {
            pComponent->setParentName(pParent->name());
        }

        pComponent->loadParameters(sBaseFile, xComponent);

        LOG_DEBUG(QString("... Adding component %1").arg(pComponent->name()));

        // Chargement des composants enfants

        QVector<CXMLNode> xChildComponents = xComponent.getNodesByTagName(ParamName_Component);

        foreach (CXMLNode xChildComponent, xChildComponents)
        {
            CComponent* pChildComponent = loadComponent(sBaseFile, pScene, xChildComponent, pComponent);

            if (pChildComponent != nullptr)
            {
                pChildComponent->setParent(QSP<CComponent>(pComponent));
            }
        }

        // Chargement du contr�leur

        CXMLNode xController = xComponent.getNodeByTagName(ParamName_Controller);

        if (xController.isEmpty() == false)
        {
            QString sControllerClass = xController.attributes()[ParamName_Class];

            CComponent* pControllerComponent = CComponentFactory::getInstance()->instanciateComponent(sControllerClass, pScene);

            if (pControllerComponent != nullptr)
            {
                pControllerComponent->loadParameters(sBaseFile, xController);

                pComponent->setController(dynamic_cast<CController*>(pControllerComponent));
                pControllerComponent->setParent(QSP<CComponent>(pComponent));
            }
        }

        return pComponent;
    }

    return nullptr;
}
