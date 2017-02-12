
// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

// Application
#include "CAirbusDataSupplier.h"

//-------------------------------------------------------------------------------------------------

CAirbusDataSupplier::CAirbusDataSupplier()
{
    LOG_DEBUG("CAirbusDataSupplier::CAirbusDataSupplier()");
}

//-------------------------------------------------------------------------------------------------

CAirbusDataSupplier::~CAirbusDataSupplier()
{
    LOG_DEBUG("CAirbusDataSupplier::~CAirbusDataSupplier()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    QVector<CXMLNode> vInputsNodes = xComponent.getNodesByTagName("DataInput");

    foreach (CXMLNode xInput, vInputsNodes)
    {
        if (xInput.attributes()[ParamName_Name].isEmpty() == false)
        {
            m_vDataInputNames.append(xInput.attributes()[ParamName_Name]);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::solveLinks(C3DScene* pScene)
{
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::clearLinks(C3DScene* pScene)
{
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::solveLinks(C3DScene* pScene, CComponent* pCaller)
{
    foreach (QString sName, m_vDataInputNames)
    {
        foreach (QSP<CComponent> pComponent, pScene->components())
        {
            QSP<CComponent> pFound = pComponent->findComponent(sName, QSP<CComponent>(pCaller));

            if (pFound)
            {
                CAirbusDataSupplier* pInput = dynamic_cast<CAirbusDataSupplier*>(pFound.data());

                if (pInput != NULL)
                {
                    m_vDataInputs.append(pInput);
                    pInput->m_vDataOutputs.append(this);
                }

                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::pushData(CAirbusData incomingData)
{
    for (int iIndex = 0; iIndex < m_vData.count(); iIndex++)
    {
        if (m_vData[iIndex].getID() == incomingData.getID() && m_vData[iIndex].getSource() == incomingData.getSource())
        {
            m_vData[iIndex].setData(incomingData.getData());
            return;
        }
    }

    m_vData.append(incomingData);
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::sendData()
{
    foreach (CAirbusDataSupplier* pConsumer, m_vDataOutputs)
    {
        sendDataToConsumer(pConsumer);
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::sendDataToConsumer(CAirbusDataSupplier* pConsumer)
{
    foreach (CAirbusData data, m_vData)
    {
        pConsumer->receiveData(data);
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusDataSupplier::receiveData(CAirbusData incomingData)
{
    for (int iIndex = 0; iIndex < m_vData.count(); iIndex++)
    {
        if (m_vData[iIndex].getID() == incomingData.getID() && m_vData[iIndex].getSource() == incomingData.getSource())
        {
            m_vData[iIndex].setData(incomingData.getData());
            return;
        }
    }

    m_vData.append(incomingData);
}

//-------------------------------------------------------------------------------------------------

CAirbusData* CAirbusDataSupplier::getData(EAirbusData eID)
{
    for (int iIndex = 0; iIndex < m_vData.count(); iIndex++)
    {
        if (m_vData[iIndex].getID() == eID)
        {
            return &(m_vData[iIndex]);
        }
    }

    return NULL;
}
