
// Application
#include "CGeneratedField.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CGeneratedField::CGeneratedField(CXMLNode xParameters)
    : m_xParameters(xParameters)
{
    // Compile parameters
    CXMLNode xHeightNode = xParameters.getNodeByTagName(ParamName_Height);
    m_pFunction = new CGenerateFunction(xParameters.getNodeByTagName(ParamName_Functions), xHeightNode.getNodeByTagName(ParamName_Value));
}

//-------------------------------------------------------------------------------------------------

CGeneratedField::~CGeneratedField()
{
    if (m_pFunction) delete m_pFunction;
}

//-------------------------------------------------------------------------------------------------

double CGeneratedField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness) *pRigidness = 1.0;

    return m_pFunction->process(CPerlin::getInstance(), gPosition.toVector3(), CAxis());
}

//-------------------------------------------------------------------------------------------------

double CGeneratedField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
    if (pRigidness) *pRigidness = 1.0;

    return m_pFunction->process(CPerlin::getInstance(), vPosition, aAxis);
}

//-------------------------------------------------------------------------------------------------

double CGeneratedField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    return m_pFunction->process(CPerlin::getInstance(), vPosition, aAxis);
}

//-------------------------------------------------------------------------------------------------

bool CGeneratedField::isGenerated()
{
    return true;
}

//-------------------------------------------------------------------------------------------------

void CGeneratedField::flatten(const CGeoloc& gPosition, double dRadius)
{
}
