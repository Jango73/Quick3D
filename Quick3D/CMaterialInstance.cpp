
#include "CMaterialInstance.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CMaterialInstance::CMaterialInstance(CMaterial* pMaterial)
: m_pMaterial(pMaterial)
{
}

//-------------------------------------------------------------------------------------------------

CMaterialInstance::~CMaterialInstance()
{
	// Don't delete the contained material
}
