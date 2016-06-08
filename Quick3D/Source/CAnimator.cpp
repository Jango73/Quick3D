
#include "C3DScene.h"
#include "CAnimator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAnimator::instanciator(C3DScene* pScene)
{
	return new CAnimator(pScene);
}

//-------------------------------------------------------------------------------------------------

CAnimator::CAnimator(C3DScene* pScene)
: CController(pScene)
, m_bIsPlaying(false)
{
}

//-------------------------------------------------------------------------------------------------

CAnimator::~CAnimator()
{
}

//-------------------------------------------------------------------------------------------------

void CAnimator::play()
{
	m_bIsPlaying = true;
}

//-------------------------------------------------------------------------------------------------

void CAnimator::stop()
{
	m_bIsPlaying = false;
}

//-------------------------------------------------------------------------------------------------

void CAnimator::loadParameters(CXMLNode xNode)
{
	CController::loadParameters(xNode);

    if (xNode.attributes()[ParamName_Playing].isEmpty() == false)
	{
        m_bIsPlaying = (bool) xNode.attributes()[ParamName_Playing].toInt();
	}
}

//-------------------------------------------------------------------------------------------------

void CAnimator::update(double dDeltaTime)
{
	CController::update(dDeltaTime);
}
