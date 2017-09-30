
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CElectricalContactor.h"

//-------------------------------------------------------------------------------------------------

CComponent* CElectricalContactor::instantiator(C3DScene* pScene)
{
	return new CElectricalContactor(pScene);
}

//-------------------------------------------------------------------------------------------------

CElectricalContactor::CElectricalContactor(C3DScene* pScene)
: CElectricalComponent(pScene)
, m_bClosed(false)
{
}

//-------------------------------------------------------------------------------------------------

CElectricalContactor::~CElectricalContactor()
{
}

//-------------------------------------------------------------------------------------------------

void CElectricalContactor::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CElectricalComponent::loadParameters(sBaseFile, xComponent);

	CXMLNode xGeneralNode = xComponent.getNodeByTagName(ParamName_General);

    if (xGeneralNode.attributes()[ParamName_Closed].isEmpty() == false)
	{
        m_bClosed = (bool) xGeneralNode.attributes()[ParamName_Closed].toInt();
	}
}

//-------------------------------------------------------------------------------------------------

void CElectricalContactor::update(double dDeltaTime)
{
	CElectricalComponent::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CElectricalContactor::push(const CElectricalLoad& tLoad, double dDeltaTime)
{
	if (m_bClosed)
	{
		CElectricalComponent::push(tLoad, dDeltaTime);
	}
}

//-------------------------------------------------------------------------------------------------

CElectricalLoad CElectricalContactor::pull(double dAmperage, double dDeltaTime)
{
	if (m_bClosed)
	{
		return CElectricalComponent::pull(dAmperage, dDeltaTime);
	}

	return CElectricalLoad(ctNone, 0.0, 0.0, 0.0);
}
