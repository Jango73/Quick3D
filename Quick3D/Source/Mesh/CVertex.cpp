
#include "CVertex.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CVertex::CVertex()
    : m_dAltitude(0.0)
    , m_dNormalDivider(0.0)
{
    m_vDiffTexWeight_0_1_2.X = 1.0;
}

//-------------------------------------------------------------------------------------------------

CVertex::CVertex(CVector3 NewPosition, CVector3 NewTexCoord)
    : m_vPosition(NewPosition)
    , m_vTexCoord(NewTexCoord)
    , m_dAltitude(0.0)
    , m_dNormalDivider(0.0)
{
    m_vDiffTexWeight_0_1_2.X = 1.0;
}

//-------------------------------------------------------------------------------------------------

CVertex::CVertex(const CVertex& target)
{
    *this = target;
}

//-------------------------------------------------------------------------------------------------

CVertex::~CVertex()
{
}

//-------------------------------------------------------------------------------------------------

CVertex& CVertex::operator=(const CVertex& target)
{
    m_vSmoothingGroups          = target.m_vSmoothingGroups;
    m_vVertexIndicesForGroup    = target.m_vVertexIndicesForGroup;
    m_vPosition                 = target.m_vPosition;
    m_vTexCoord                 = target.m_vTexCoord;
    m_vNormal                   = target.m_vNormal;
    m_vTangent                  = target.m_vTangent;
    m_vGravity                  = target.m_vGravity;
    m_dAltitude                 = target.m_dAltitude;
    m_dNormalDivider            = target.m_dNormalDivider;
    m_vDiffTexWeight_0_1_2      = target.m_vDiffTexWeight_0_1_2;
    m_vDiffTexWeight_3_4_5      = target.m_vDiffTexWeight_3_4_5;
    m_vDiffTexWeight_6_7_8      = target.m_vDiffTexWeight_6_7_8;

    return *this;
}

//-------------------------------------------------------------------------------------------------

void CVertex::setDiffuseTextureWeight(int iTexture, double dWeight)
{
    if (iTexture == 0) m_vDiffTexWeight_0_1_2.X = dWeight;
    else if (iTexture == 1) m_vDiffTexWeight_0_1_2.Y = dWeight;
    else if (iTexture == 2) m_vDiffTexWeight_0_1_2.Z = dWeight;
    else if (iTexture == 3) m_vDiffTexWeight_3_4_5.X = dWeight;
    else if (iTexture == 4) m_vDiffTexWeight_3_4_5.Y = dWeight;
    else if (iTexture == 5) m_vDiffTexWeight_3_4_5.Z = dWeight;
    else if (iTexture == 6) m_vDiffTexWeight_6_7_8.X = dWeight;
    else if (iTexture == 7) m_vDiffTexWeight_6_7_8.Y = dWeight;
}
