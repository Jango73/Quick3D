
#include "CGeoBounds.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CGeoBounds::CGeoBounds(CGeoloc gPosition, const CBoundingBox& bSize, double dHeading)
    : m_gPosition(gPosition)
    , m_bSize(bSize)
{
    compute();
}

//-------------------------------------------------------------------------------------------------

CGeoBounds::~CGeoBounds()
{
}

//-------------------------------------------------------------------------------------------------

Math::RayTracingResult CGeoBounds::intersect(Math::CRay3 ray)
{
    RayTracingResult tResult(Q3D_INFINITY);

    return tResult;
}

//-------------------------------------------------------------------------------------------------

bool CGeoBounds::contains(CGeoloc gPosition) const
{
    return false;
}

//-------------------------------------------------------------------------------------------------

void CGeoBounds::compute()
{
}
