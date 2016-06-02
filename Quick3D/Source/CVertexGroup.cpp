
// Fondations
#include "CLogManager.h"

// Application
#include "CVertexGroup.h"

//-------------------------------------------------------------------------------------------------

CVertexGroup::CVertexGroup()
{
}

//-------------------------------------------------------------------------------------------------

CVertexGroup::~CVertexGroup()
{
}

//-------------------------------------------------------------------------------------------------

double CVertexGroup::vertexWeight(int iIndex) const
{
	if (m_vWeights.contains(iIndex))
	{
		return m_vWeights[iIndex];
	}

	return 0.0;
}
